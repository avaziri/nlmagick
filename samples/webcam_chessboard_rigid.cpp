#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <fstream>

#include <nlopt/nlopt.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace cv;
using namespace std;
using namespace nlopt;

#define foreach         BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

namespace po = boost::program_options;
using boost::lexical_cast;

namespace {
struct Options {
    std::string k_file;
    int width, height;
    float square_size;
    int vid;
};

int options(int ac, char ** av, Options& opts) {
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()("help", "Produce help message.")(
            "intrinsics,K",
            po::value<string>(&opts.k_file),
            "The camera intrinsics file, should be yaml and have atleast \"K:...\". Required.")(
            "width,W", po::value<int>(&opts.width)->default_value(8),
            "The number of inside corners, width wise of chessboard.")(
            "height,H", po::value<int>(&opts.height)->default_value(6),
            "The number of inside corners, height wise of chessboard.")(
            "square_size,S",
            po::value<float>(&opts.square_size)->default_value(1),
            "The size of each square in meters.")("video,V", po::value<int>(
            &opts.vid)->default_value(-1),
            "Video device number, find video by ls /dev/video*.");

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (!vm.count("intrinsics")) {
        cout << "Must supply a camera calibration file" << "\n";
        cout << desc << endl;
        return 1;
    }
    return 0;

}

}

class RigidTransformFitter: public OptimProblem {
public:
    RigidTransformFitter() {
    }
    virtual ~RigidTransformFitter() {
    }

    double evalCostFunction(const double* X, double*) {

        std::memcpy(w_est.data, X, 3 * sizeof(double));
        std::memcpy(T_est.data, X + 3, 3 * sizeof(double));

        Rodrigues(w_est, R_est); //get a rotation matrix

        transform(xyz1, xyz1_hat, R_est); //rotate 3d points
        xyz1_hat += cv::repeat(T_est.reshape(3, 1), 1, xyz1_hat.cols); //translate

        transform(xyz1_hat, uv2hat, K); //project
        {
            //normalize
            Mat ch[3];
            split(uv2hat, ch);
            Mat uv2_t;
            multiply(ch[0], 1.0 / ch[2], ch[0]); //divide by third channel
            multiply(ch[1], 1.0 / ch[2], ch[1]); //divide by third channel
            merge(ch, 2, uv2_t); //merge the two channels back.
            uv2hat = uv2_t;
        }

        double fval = norm(uv2 - uv2hat, cv::NORM_L2); // minimize reprojection error!
        fval += 1.0e-2 * norm(w_est);                  // regularize: shrink omega!
        fval += exp( -CV_PI + abs(X[0]) ) + exp( -CV_PI + abs(X[1])) + exp( -CV_PI + abs(X[2]));
        return fval;
    }

    virtual size_t N() const {
        return 6; //6 free params, 3 for R 3 for T
    }


    virtual OptimAlgorithm getAlgorithm() const {
        //http://ab-initio.mit.edu/wiki/index.php/NLopt_Algorithms#Nelder-Mead_Simplex
        return NLOPT_LN_NELDERMEAD;
    }

    void setup(const std::vector<cv::Mat>& input_data) {

        // what we're allowed to see: image points and calibration K matrix
        xyz1 = input_data[0].clone();
        uv2 = input_data[1].clone();
        K = input_data[2].clone();

        T_est = cv::Mat::zeros(3, 1, CV_64F);
        w_est = cv::Mat::zeros(3, 1, CV_64F);
    }

public:
    // some internal persistent data
    cv::Mat K; //camera matrix
    cv::Mat uv2; //observation points
    cv::Mat xyz1; //ideal 3d points
    Mat xyz1_hat; // transformed 3d points
    Mat uv2hat; //projected ideal points by R,T
    // solving for these
    cv::Mat T_est;
    cv::Mat w_est;
    cv::Mat R_est;

};

vector<Point3f> CalcChessboardCorners(cv::Size chess_size, float square_size) {
    vector<Point3f> corners;
    corners.reserve(chess_size.area());

    for (int i = 0; i < chess_size.height; i++)
        for (int j = 0; j < chess_size.width; j++)
            corners.push_back(Point3f(float(j * square_size), float(i
                    * square_size), 1));
    return corners;
}

int main(int argc, char** argv) {
    /*
     Create some 3D Points, X
     Project to image 1:  fXx/Xz + ox ,  fXy/Xz + oy

     Rotate & Translate 3D points:  Y = RX + T
     Project to image 2:  fYx/Yz + ox ,  fYy/Yz + oy

     Given the correspondence of points, reconstruct R & T!

     */
    Options opts;
    if (options(argc, argv, opts))
        return 1;

    Camera camera(opts.k_file);
    Size board_size(opts.width, opts.height);

    cout << "testing nlopt ... " << endl;

    vector<Point3f> template_board = CalcChessboardCorners(board_size,
            opts.square_size);

    cout << "K = " << camera.K() << endl;
    cout << "board_points " << template_board << endl;
    vector<Point2f> imgpts;
    projectPoints(Mat(template_board), Mat::zeros(3, 1, CV_32F), Mat::zeros(3,
            1, CV_32F), camera.K(), Mat(), imgpts);
    cout << "image points " << imgpts << endl;
    Mat xyz1(template_board);
    xyz1 = xyz1.t();
    Mat uv1(imgpts);
    uv1 = uv1.t();
    //  cout << "xyz1 = " << xyz1 << " ; " << endl;
    cout << "uv1 = " << uv1 << " ; " << endl;

    boost::shared_ptr<RigidTransformFitter> rigid_test_problem(
            new RigidTransformFitter());
    vector<Mat> data(3);
    xyz1.convertTo(data[0], CV_64F); //ideal 3d points
    uv1.convertTo(data[1], CV_64F); // init with the  projected version of the chessboard
    camera.K().convertTo(data[2], CV_64F);

    // lock and load
    rigid_test_problem->setup(data);

    NLOptCore opt_core(rigid_test_problem);

    opt_core.optimize();
    // evaluate body count
    vector<double> optimal_W_and_T = opt_core.getOptimalVector();
    double fval_final = opt_core.getFunctionValue();
    cout << "result code: " << opt_core.getResult() << endl;
    cout << " [w,T] optimal = " << Mat(optimal_W_and_T) << endl;
    cout << " ground truth  = " << Mat::zeros(1, 3, CV_32F) << ", "
            << Mat::zeros(1, 3, CV_32F) << endl;
    cout << " error = " << fval_final << endl;
    if (opts.vid < 0)
        return 0;
    VideoCapture capture(opts.vid);
    if (!capture.isOpened()) {
        cerr << "unable to open video device " << opts.vid;
        return 1;
    }
    Mat frame;
    vector<Point2f> observation;
    for (;;) {
        capture >> frame;
        if (frame.empty())
            continue;
        // TODO: fork and do this until we find one
        bool found = findChessboardCorners(frame, board_size, observation,
                CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_FAST_CHECK
                        + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FILTER_QUADS);
        // draw previous one or something until fork returns        
        
        drawChessboardCorners(frame, board_size, observation, found);
        if (found) {
            Mat uv2(Mat(observation).t());
            uv2.convertTo(rigid_test_problem->uv2, CV_64F);

            opt_core.optimize(&(optimal_W_and_T[0]));
            for( int k = 0; k < 3; k++ ) {
              while( optimal_W_and_T[k] < -CV_PI ) {
                optimal_W_and_T[k] += 2*CV_PI;
              }
              while( optimal_W_and_T[k] > CV_PI ) {
                optimal_W_and_T[k] -= 2*CV_PI;
              }
            }

            optimal_W_and_T = opt_core.getOptimalVector();
            fval_final = opt_core.getFunctionValue();

            cout << "result code: " << opt_core.getResult() << endl;
            cout << " [w,T] optimal = " << Mat(optimal_W_and_T) << endl;
            cout << " error = " << fval_final << endl;
        }
        imshow("frame", frame);
        char key = cv::waitKey(10);
        if( 'q' == key )
          break;

    }

    return 0;

}
