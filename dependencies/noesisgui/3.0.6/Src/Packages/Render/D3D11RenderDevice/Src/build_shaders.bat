@echo off

if exist Shaders.tar del Shaders.tar
if exist Shaders.h del Shaders.h

:: Vertex Shaders

CALL :fxc_vs Pos_VS ShaderVS.hlsl /DHAS_POSITION
CALL :fxc_vs PosColor_VS ShaderVS.hlsl /DHAS_POSITION /DHAS_COLOR
CALL :fxc_vs PosTex0_VS ShaderVS.hlsl /DHAS_POSITION /DHAS_UV0
CALL :fxc_vs PosColorCoverage_VS ShaderVS.hlsl /DHAS_POSITION /DHAS_COLOR /DHAS_COVERAGE
CALL :fxc_vs PosTex0Coverage_VS ShaderVS.hlsl /DHAS_POSITION /DHAS_UV0 /DHAS_COVERAGE
CALL :fxc_vs PosColorTex1_VS ShaderVS.hlsl /DHAS_POSITION /DHAS_COLOR /DHAS_UV1
CALL :fxc_vs PosTex0Tex1_VS ShaderVS.hlsl /DHAS_POSITION /DHAS_UV0 /DHAS_UV1
CALL :fxc_vs PosColorTex1Tex2_VS ShaderVS.hlsl /DHAS_POSITION /DHAS_COLOR /DHAS_UV1 /DHAS_UV2
CALL :fxc_vs PosTex0Tex1Tex2_VS ShaderVS.hlsl /DHAS_POSITION /DHAS_UV0 /DHAS_UV1 /DHAS_UV2
CALL :fxc_vs PosColorTex1_SDF_VS ShaderVS.hlsl /DHAS_POSITION /DHAS_COLOR /DHAS_UV1 /DGEN_ST1
CALL :fxc_vs PosTex0Tex1_SDF_VS ShaderVS.hlsl /DHAS_POSITION /DHAS_UV0 /DHAS_UV1 /DGEN_ST1

:: Pixel Shaders

CALL :fxc_ps RGBA_FS ShaderPS.hlsl /DEFFECT_RGBA
CALL :fxc_ps Mask_FS ShaderPS.hlsl /DEFFECT_MASK

CALL :fxc_ps PathSolid_FS ShaderPS.hlsl /DEFFECT_PATH_SOLID
CALL :fxc_ps PathLinear_FS ShaderPS.hlsl /DEFFECT_PATH_LINEAR
CALL :fxc_ps PathRadial_FS ShaderPS.hlsl /DEFFECT_PATH_RADIAL
CALL :fxc_ps PathPattern_FS ShaderPS.hlsl /DEFFECT_PATH_PATTERN

CALL :fxc_ps PathAASolid_FS ShaderPS.hlsl /DEFFECT_PATH_AA_SOLID
CALL :fxc_ps PathAALinear_FS ShaderPS.hlsl /DEFFECT_PATH_AA_LINEAR
CALL :fxc_ps PathAARadial_FS ShaderPS.hlsl /DEFFECT_PATH_AA_RADIAL
CALL :fxc_ps PathAAPattern_FS ShaderPS.hlsl /DEFFECT_PATH_AA_PATTERN

CALL :fxc_ps SDFSolid_FS ShaderPS.hlsl /DEFFECT_SDF_SOLID
CALL :fxc_ps SDFLinear_FS ShaderPS.hlsl /DEFFECT_SDF_LINEAR
CALL :fxc_ps SDFRadial_FS ShaderPS.hlsl /DEFFECT_SDF_RADIAL
CALL :fxc_ps SDFPattern_FS ShaderPS.hlsl /DEFFECT_SDF_PATTERN

CALL :fxc_ps SDFLCDSolid_FS ShaderPS.hlsl /DEFFECT_SDF_LCD_SOLID
CALL :fxc_ps SDFLCDLinear_FS ShaderPS.hlsl /DEFFECT_SDF_LCD_LINEAR
CALL :fxc_ps SDFLCDRadial_FS ShaderPS.hlsl /DEFFECT_SDF_LCD_RADIAL
CALL :fxc_ps SDFLCDPattern_FS ShaderPS.hlsl /DEFFECT_SDF_LCD_PATTERN

CALL :fxc_ps ImageOpacitySolid_FS ShaderPS.hlsl /DEFFECT_IMAGE_OPACITY_SOLID
CALL :fxc_ps ImageOpacityLinear_FS ShaderPS.hlsl /DEFFECT_IMAGE_OPACITY_LINEAR
CALL :fxc_ps ImageOpacityRadial_FS ShaderPS.hlsl /DEFFECT_IMAGE_OPACITY_RADIAL
CALL :fxc_ps ImageOpacityPattern_FS ShaderPS.hlsl /DEFFECT_IMAGE_OPACITY_PATTERN

CALL :fxc_ps ImageShadow35V_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_35V
CALL :fxc_ps ImageShadow63V_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_63V
CALL :fxc_ps ImageShadow127V_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_127V

CALL :fxc_ps ImageShadow35HSolid_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_35H_SOLID
CALL :fxc_ps ImageShadow35HLinear_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_35H_LINEAR
CALL :fxc_ps ImageShadow35HRadial_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_35H_RADIAL
CALL :fxc_ps ImageShadow35HPattern_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_35H_PATTERN

CALL :fxc_ps ImageShadow63HSolid_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_63H_SOLID
CALL :fxc_ps ImageShadow63HLinear_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_63H_LINEAR
CALL :fxc_ps ImageShadow63HRadial_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_63H_RADIAL
CALL :fxc_ps ImageShadow63HPattern_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_63H_PATTERN

CALL :fxc_ps ImageShadow127HSolid_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_127H_SOLID
CALL :fxc_ps ImageShadow127HLinear_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_127H_LINEAR
CALL :fxc_ps ImageShadow127HRadial_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_127H_RADIAL
CALL :fxc_ps ImageShadow127HPattern_FS ShaderPS.hlsl /DEFFECT_IMAGE_SHADOW_127H_PATTERN

CALL :fxc_ps ImageBlur35V_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_35V
CALL :fxc_ps ImageBlur63V_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_63V
CALL :fxc_ps ImageBlur127V_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_127V

CALL :fxc_ps ImageBlur35HSolid_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_35H_SOLID
CALL :fxc_ps ImageBlur35HLinear_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_35H_LINEAR
CALL :fxc_ps ImageBlur35HRadial_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_35H_RADIAL
CALL :fxc_ps ImageBlur35HPattern_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_35H_PATTERN

CALL :fxc_ps ImageBlur63HSolid_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_63H_SOLID
CALL :fxc_ps ImageBlur63HLinear_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_63H_LINEAR
CALL :fxc_ps ImageBlur63HRadial_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_63H_RADIAL
CALL :fxc_ps ImageBlur63HPattern_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_63H_PATTERN

CALL :fxc_ps ImageBlur127HSolid_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_127H_SOLID
CALL :fxc_ps ImageBlur127HLinear_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_127H_LINEAR
CALL :fxc_ps ImageBlur127HRadial_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_127H_RADIAL
CALL :fxc_ps ImageBlur127HPattern_FS ShaderPS.hlsl /DEFFECT_IMAGE_BLUR_127H_PATTERN

:: Clear and Resolve 

CALL :fxc_vs Quad_VS QuadVS.hlsl
CALL :fxc_ps Clear_PS ClearPS.hlsl
CALL :fxc_ps Resolve2_PS ResolvePS.hlsl "/DNUM_SAMPLES=2"
CALL :fxc_ps Resolve4_PS ResolvePS.hlsl "/DNUM_SAMPLES=4"
CALL :fxc_ps Resolve8_PS ResolvePS.hlsl "/DNUM_SAMPLES=8"
CALL :fxc_ps Resolve16_PS ResolvePS.hlsl "/DNUM_SAMPLES=16"

:: Compress
Pack Shaders.tar Shaders.z
bin2h.py Shaders.z Shaders >> Shaders.h

del Shaders.z
del Shaders.tar

EXIT /B 0

:: -------------------------------------------------------------------------------------------------------
:fxc_vs
    fxc /T vs_4_0 /nologo /O3 /Qstrip_reflect /Fo %1.o %2 %3 %4 %5 %6 > NUL
    if %errorlevel% neq 0 EXIT /B %errorlevel%
    tar.py Shaders.tar %1.o >> Shaders.h
    del %1.o
    EXIT /B 0

:fxc_ps
    fxc /T ps_4_0 /nologo /O3 /Qstrip_reflect /Fo %1.o %2 %3 %4 %5 %6 > NUL
    if %errorlevel% neq 0 EXIT /B %errorlevel%
    tar.py Shaders.tar %1.o >> Shaders.h
    del %1.o
    EXIT /B 0