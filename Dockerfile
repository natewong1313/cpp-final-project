# using same env as wsl
FROM ubuntu:22.04
# install the deps
RUN apt-get update
RUN apt-get install -y libsqlite3-dev build-essential cmake

WORKDIR /app
COPY . .
# have to make the build directory first since we dockerignored it
RUN mkdir build
# cant run the makefile bc it runs the executable
RUN cd build && cmake .. && cmake --build .

EXPOSE 8080
# need to fix sql path issues
CMD ["bash", "-c", "cd build && ./main"]