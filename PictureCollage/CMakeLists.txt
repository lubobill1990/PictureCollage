# Some flags for Freeglut and GLUI.
add_definitions( -DFREEGLUT_EXPORTS -DFREEGLUT_STATIC -D_CRT_SECURE_NO_WARNINGS )

# Define the framework files.
set(PictureCollage_Framework_SRCS
	Framework/Main.cpp
	Framework/Render.cpp
	Framework/Render.h
	Framework/Test.cpp
	Framework/Test.h
)

#define the test files.
set(PictureCollage_Tests_SRCS
    Tests/BodyData.h
    Tests/BodyTypes.h
    Tests/Common.cpp
    Tests/Common.h
    Tests/FixtureData.cpp
    Tests/FixtureData.h
    Tests/HandleLayoutResult.cpp
    Tests/HandleLayoutResult.h
    Tests/PicsContactFilter.cpp
    Tests/PicsContactFilter.h
    Tests/PolyShapes.cpp
    Tests/PolyShapes.h
    Tests/TestEntries.cpp
    )
set(PictureCollage_Polygon_SRCS
    Polygon/Basic.h
    Polygon/Common.h
    Polygon/Common.cpp
    Polygon/Polygon.h
    Polygon/LineSegment.cpp
    Polygon/Polygon.cpp
    Polygon/PolygonList.cpp
    Polygon/Screen.h
    )
set(PictureCollage_IO_SRCS
    IO/Picture.h
    IO/Picture.cpp
    IO/PictureIO.h
    IO/PictureIO.cpp
    )
# These are used to create visual studio folders.
source_group(Framework FILES ${PictureCollage_Framework_SRCS})
source_group(Tests FILES ${PictureCollage_Tests_SRCS})
source_group(Polygon FILES ${PictureCollage_Polygon_SRCS})
source_group(IO FILES ${PictureCollage_IO_SRCS})

include_directories (
    ${OPENGL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${BOOST_ROOT}
    )

if(APPLE)
    # We are not using the Apple's framework version, but X11's
    include_directories( /usr/X11/include )
    link_directories( /usr/X11/lib )
    set (OPENGL_LIBRARIES GL GLU GLUT X11)
endif(APPLE)

add_executable(PictureCollage
    ${PictureCollage_Framework_SRCS}
    ${PictureCollage_Tests_SRCS}
    ${PictureCollage_Polygon_SRCS}
    ${PictureCollage_IO_SRCS}
    )

target_link_libraries (
    PictureCollage
    Box2D
    freeglut_static
    glui
    ${OPENGL_LIBRARIES}
    ) 
