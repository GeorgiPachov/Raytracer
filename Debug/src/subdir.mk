################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/bitmap.cpp \
../src/camera.cpp \
../src/geometry.cpp \
../src/main.cpp \
../src/matrix.cpp \
../src/sdl.cpp \
../src/shading.cpp 

OBJS += \
./src/bitmap.o \
./src/camera.o \
./src/geometry.o \
./src/main.o \
./src/matrix.o \
./src/sdl.o \
./src/shading.o 

CPP_DEPS += \
./src/bitmap.d \
./src/camera.d \
./src/geometry.d \
./src/main.d \
./src/matrix.d \
./src/sdl.d \
./src/shading.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


