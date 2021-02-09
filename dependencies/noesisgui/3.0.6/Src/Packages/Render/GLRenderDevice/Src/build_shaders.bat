@echo off

CALL :validate_vertex Shader.120.vert Shader_120_vert
CALL :validate_vertex Shader.140.vert Shader_140_vert
CALL :validate_vertex Shader.100es.vert Shader_100es_vert
CALL :validate_vertex Shader.300es.vert Shader_300es_vert

CALL :validate_fragment Shader.120.frag Shader_120_frag
CALL :validate_fragment Shader.140.frag Shader_140_frag
CALL :validate_fragment Shader.100es.frag Shader_100es_frag
CALL :validate_fragment Shader.300es.frag Shader_300es_frag

goto :eof

:: -------------------------------------------------------------------------------------------------------
:validate_vertex
    echo %1

    CALL :validate %1 || goto :eof
    CALL :validate -DHAS_COLOR %1 || goto :eof
    CALL :validate -DHAS_UV0 %1 || goto :eof
    CALL :validate -DHAS_COLOR -DHAS_COVERAGE %1 || goto :eof
    CALL :validate -DHAS_UV0 -DHAS_COVERAGE %1 || goto :eof
    CALL :validate -DHAS_COLOR -DHAS_UV1 %1 || goto :eof
    CALL :validate -DHAS_UV0 -DHAS_UV1 %1 || goto :eof
    CALL :validate -DHAS_COLOR -DHAS_UV1 -DHAS_UV2 %1 || goto :eof
    CALL :validate -DHAS_UV0 -DHAS_UV1 -DHAS_UV2 %1 || goto :eof
    CALL :validate -DHAS_COLOR -DHAS_UV1 -DHAS_ST1 %1 || goto :eof
    CALL :validate -DHAS_UV0 -DHAS_UV1 -DHAS_ST1 %1 || goto :eof

    shader2h.py %1 %2 > %1.h

    goto :eof

:: -------------------------------------------------------------------------------------------------------
:validate_fragment
    echo %1

    CALL :validate -DEFFECT_RGBA %1 || goto :eof
    CALL :validate -DEFFECT_MASK %1 || goto :eof

    CALL :validate -DEFFECT_PATH_SOLID %1 || goto :eof
    CALL :validate -DEFFECT_PATH_LINEAR %1 || goto :eof
    CALL :validate -DEFFECT_PATH_RADIAL %1 || goto :eof
    CALL :validate -DEFFECT_PATH_PATTERN %1 || goto :eof

    CALL :validate -DEFFECT_PATH_AA_SOLID %1 || goto :eof
    CALL :validate -DEFFECT_PATH_AA_LINEAR %1 || goto :eof
    CALL :validate -DEFFECT_PATH_AA_RADIAL %1 || goto :eof
    CALL :validate -DEFFECT_PATH_AA_PATTERN %1 || goto :eof

    CALL :validate -DEFFECT_SDF_SOLID %1 || goto :eof
    CALL :validate -DEFFECT_SDF_LINEAR %1 || goto :eof
    CALL :validate -DEFFECT_SDF_RADIAL %1 || goto :eof
    CALL :validate -DEFFECT_SDF_PATTERN %1 || goto :eof

    CALL :validate -DEFFECT_IMAGE_OPACITY_SOLID %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_OPACITY_LINEAR %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_OPACITY_RADIAL %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_OPACITY_PATTERN %1 || goto :eof

    CALL :validate -DEFFECT_IMAGE_SHADOW_35V %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_SHADOW_63V %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_SHADOW_127V %1 || goto :eof

    CALL :validate -DEFFECT_IMAGE_SHADOW_35H_SOLID %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_SHADOW_35H_LINEAR %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_SHADOW_35H_RADIAL %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_SHADOW_35H_PATTERN %1 || goto :eof

    CALL :validate -DEFFECT_IMAGE_SHADOW_63H_SOLID %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_SHADOW_63H_LINEAR %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_SHADOW_63H_RADIAL %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_SHADOW_63H_PATTERN %1 || goto :eof

    CALL :validate -DEFFECT_IMAGE_SHADOW_127H_SOLID %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_SHADOW_127H_LINEAR %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_SHADOW_127H_RADIAL %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_SHADOW_127H_PATTERN %1 || goto :eof

    CALL :validate -DEFFECT_IMAGE_BLUR_35V %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_BLUR_63V %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_BLUR_127V %1 || goto :eof

    CALL :validate -DEFFECT_IMAGE_BLUR_35H_SOLID %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_BLUR_35H_LINEAR %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_BLUR_35H_RADIAL %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_BLUR_35H_PATTERN %1 || goto :eof

    CALL :validate -DEFFECT_IMAGE_BLUR_63H_SOLID %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_BLUR_63H_LINEAR %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_BLUR_63H_RADIAL %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_BLUR_63H_PATTERN %1 || goto :eof

    CALL :validate -DEFFECT_IMAGE_BLUR_127H_SOLID %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_BLUR_127H_LINEAR %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_BLUR_127H_RADIAL %1 || goto :eof
    CALL :validate -DEFFECT_IMAGE_BLUR_127H_PATTERN %1 || goto :eof

    shader2h.py %1 %2 > %1.h

    goto :eof

:: -------------------------------------------------------------------------------------------------------
:validate
    glslangValidator.exe --quiet -l %*
    if %errorlevel% neq 0 EXIT /B %errorlevel%
    goto :eof