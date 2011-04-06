AG_PROJECT(SHADERCOMPILER SCP)
AG_FILE(SHADERCOMPILER_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${SHADERCOMPILER_FILES})

#-----------------------------------------------------------------------------
IF( WIN32 )
   SET( WIN32_EXECUTABLE WIN32 )
ENDIF( WIN32 )

add_executable (ShaderCompiler ${WIN32_EXECUTABLE} ${SHADERCOMPILER_FILES})

use_precompiled_header (ShaderCompiler)

target_link_libraries (ShaderCompiler RendererSM2 GfxDx9 Window winmm)
