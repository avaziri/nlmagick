macro(data_download PATH_VAR DATA_FILE SAVE_TO_PATH)
  set(data_base_url http://www.theveganrobot.com/nlmagick/data)
  set(${PATH_VAR} ${PROJECT_BINARY_DIR}/${SAVE_TO_PATH}/${DATA_FILE})
# if(NOT EXISTS ${${PATH_VAR}})
#    message(STATUS "Data fetch.\n** Downloading:\n** ${data_base_url}/${DATA_FILE}\n** to:\n** ${${PATH_VAR}}")
#    file(DOWNLOAD ${data_base_url}/${DATA_FILE} ${${PATH_VAR}})
 # endif()
endmacro()


add_executable(nlmagick_test_fmincon  nlmagick_test_fmincon.cpp)
target_link_libraries(nlmagick_test_fmincon  nlmagick ${Boost_LIBRARIES})

add_test(test_fmincon_A   ${CMAKE_BINARY_DIR}/bin/nlmagick_test_fmincon -x 20 -y 20 -m 10)

if(FALSE)  # server might be broken !
    # Rigid Image Match = 640x480
    data_download(image_0000 image_0000.jpg data_rigid_template)
    data_download(image_0001 image_0001.jpg data_rigid_template)
    data_download(image_0002 image_0002.jpg data_rigid_template)
    data_download(image_mask image_mask.jpg data_rigid_template)

    # 480x360 images
    data_download(image_mask_Ax  image_mask_Ax.jpg data_rigid_template)
    data_download(image_0000_Ax  image_0000_Ax.jpg data_rigid_template)
    data_download(image_0001_Ax  image_0001_Ax.jpg data_rigid_template)
    data_download(image_0002_Ax  image_0002_Ax.jpg data_rigid_template)

    # 320x240 images
    data_download(image_mask_2x  image_mask_2x.jpg data_rigid_template)
    data_download(image_0000_2x  image_0000_2x.jpg data_rigid_template)
    data_download(image_0001_2x  image_0001_2x.jpg data_rigid_template)
    data_download(image_0002_2x  image_0002_2x.jpg data_rigid_template)

    # 160x120 images
    data_download(image_mask_3x  image_mask_3x.jpg data_rigid_template)
    data_download(image_0000_3x  image_0000_3x.jpg data_rigid_template)
    data_download(image_0001_3x  image_0001_3x.jpg data_rigid_template)
    data_download(image_0002_3x  image_0002_3x.jpg data_rigid_template)
endif()

add_test(test_image_rigid_LinuxFlag
    ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_640x480.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000.jpg
    -a NLOPT_LN_BOBYQA
    -w warped01blur1.jpg
    -s 25
    -f blurredAnswer1.out
  )

add_test(test_image_rigid_LinuxFlag_Frame2
    ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_640x480.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0002.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000.jpg
    -a NLOPT_LN_BOBYQA
    -w warped02blur1.jpg
    -s 25
    -g blurredAnswer1.out
    -f blurredAnswer2.out
  )

add_test(test_image_rigid_LinuxFlag_480x360
    ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_480x360.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001_Ax.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask_Ax.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000_Ax.jpg
    -a NLOPT_LN_BOBYQA
    -s 25
    -w warped01blur1_Ax.jpg
    -f blurredAnswer1_Ax.out
    -v 2 # display
  )


add_test(test_image_rigid_LinuxFlag_320x240
    ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_320x240.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001_2x.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask_2x.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000_2x.jpg
    -a NLOPT_LN_BOBYQA
    -s 15
    -L 3
    -w warped01blur1_2x.jpg
    -f blurredAnswer1_2x.out
    -v 2 # display
  )

add_test(test_image_rigid_LinuxFlag_160x120
    ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_160x120.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001_3x.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask_3x.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000_3x.jpg
    -a NLOPT_LN_BOBYQA
    -s 7
    -w warped01blur1_3x.jpg
    -L 2
    -g blurredAnswer1_2x.out
    -f blurredAnswer1_3x.out
    -v 2 # display
  )

add_test(test_image_rigid_LinuxFlag_NoDisplay_CoreSpeedTest_640x480
    ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_640x480.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000.jpg
    -a NLOPT_LN_BOBYQA
    -w warped01blur1.jpg
    -s 25
    -f blurredAnswer1.out
    -v 0 # no display, test speed
  )

add_test(test_image_rigid_LinuxFlag_NoDisplay_CoreSpeedTest_640x480_CloseInit
    ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_640x480.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000.jpg
    -a NLOPT_LN_BOBYQA
    -w warped01blur2.jpg
    -s 5
    -g blurredAnswer1.out  # Feedback the previous output, should converge blazing fast
    -f blurredAnswer2.out
    -v 0 # no display, test speed
  )


add_test(test_image_rigid_LinuxFlag_NoDisplay_CoreSpeedTest_480x360
    ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_480x360.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001_Ax.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask_Ax.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000_Ax.jpg
    -a NLOPT_LN_BOBYQA
    -s 25
    -f blurredAnswer1_Ax.out
    -v 0 # no display, test speed
  )

add_test(test_image_rigid_LinuxFlag_NoDisplay_CoreSpeedTest_480x360_CloseInit
    ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_480x360.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001_Ax.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask_Ax.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000_Ax.jpg
    -a NLOPT_LN_BOBYQA
    -s 5
    -f blurredAnswer2_Ax.out
    -g blurredAnswer1_Ax.out
    -v 0 # no display, test speed
  )

add_test(test_image_rigid_LinuxFlag_NoDisplay_CoreSpeedTest_320x240
    ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_320x240.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001_2x.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask_2x.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000_2x.jpg
    -a NLOPT_LN_BOBYQA
    -s 15
    -f blurredAnswer1_2x.out
    -v 0 # no display, test speed
  )

add_test(test_image_rigid_LinuxFlag_NoDisplay_CoreSpeedTest_320x240_CloseInit
    ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_320x240.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001_2x.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask_2x.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000_2x.jpg
    -a NLOPT_LN_BOBYQA
    -s 5
    -g blurredAnswer1_2x.out
    -f blurredAnswer2_2x.out
    -v 0 # no display, test speed
  )


add_test(test_image_rigid_LinuxFlag_NoDisplay_CoreSpeedTest_160x120
        ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_160x120.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001_3x.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask_3x.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000_3x.jpg
    -a NLOPT_LN_BOBYQA
    -s 7
    -w warped01blur1_3x.jpg
    -L 2
    -f blurredAnswer1_3x.out
    -v 0 # no display, test speed
  )

add_test(test_image_rigid_LinuxFlag_NoDisplay_CoreSpeedTest_160x120_CloseInit
        ${CMAKE_BINARY_DIR}/bin/template_image_rigid 
    -K ${CMAKE_SOURCE_DIR}/testdata/camera_droidx_160x120.yml
    -i ${CMAKE_BINARY_DIR}/data_rigid_template/image_0001_3x.jpg
    -m ${CMAKE_BINARY_DIR}/data_rigid_template/image_mask_3x.jpg
    -t ${CMAKE_BINARY_DIR}/data_rigid_template/image_0000_3x.jpg
    -a NLOPT_LN_BOBYQA
    -s 3
    -w warped01blur1_3x.jpg
    -L 2
    -f blurredAnswer2_3x.out
    -g blurredAnswer1_3x.out
    -v 0 # no display, test speed
  )




data_download(trees0 trees160.png data_trees_af2f)
data_download(trees1 trees161.png data_trees_af2f)
data_download(trees2 trees162.png data_trees_af2f)
data_download(trees3 trees163.png data_trees_af2f)
data_download(trees4 trees164.png data_trees_af2f)
data_download(trees5 trees165.png data_trees_af2f)
data_download(trees6 trees166.png data_trees_af2f)
data_download(trees7 trees167.png data_trees_af2f)
data_download(trees8 trees168.png data_trees_af2f)
data_download(trees9 trees169.png data_trees_af2f)

add_test(test_affineFrame2Frame
    ${CMAKE_BINARY_DIR}/bin/affine_frame2frame
    -d ${CMAKE_BINARY_DIR}/data_trees_af2f
    -v 1
  )
