INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/inc
					${CMAKE_CURRENT_SOURCE_DIR}/src
					${CMAKE_SOURCE_DIR}/thirdparty/Poco/poco-1.7.4-win/Net/include
					)


LIST(APPEND apollo_app_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/configure_client.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/src/configure_listener.cxx
	#${CMAKE_CURRENT_SOURCE_DIR}/src/apollo_app.cxx
	)

FILE(GLOB INCS "inc/*.h")
FILE(GLOB HEADERS "src/*.h")
SOURCE_GROUP("Headers Files" FILES ${INCS} ${HEADERS})
ADD_DEFINITIONS(/D"_WIN32_WINNT=0x600")
#ADD_EXECUTABLE(apollo_client ${apollo_app_SOURCES} ${HEADERS} ${INCS})
ADD_LIBRARY(apollo_client STATIC ${apollo_app_SOURCES} ${HEADERS} ${INCS})

#ADD_DEFINITIONS(-DWPCAP -DHAVE_REMOTE /D"WEBRTC_WIN" /D"NOMINMAX")
TARGET_LINK_LIBRARIES(apollo_client 
						ws2_32
						Winmm
						PocoUtil 
						PocoXML 
						PocoJSON 
						PocoFoundation
						PocoNet
						)
SET_TARGET_PROPERTIES(apollo_client PROPERTIES FOLDER "apollo")
