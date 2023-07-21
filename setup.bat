mkdir libs
cd libs

cmake -S ../vendor/glfw -G "MinGW Makefiles" -B glfw -D BUILD_SHARED_LIBS=OFF -D GLFW_BUILD_EXAMPLES=OFF -D GLFW_BUILD_TESTS=OFF

cd glfw && make
cd ../

mv glfw/src/libglfw3.a .