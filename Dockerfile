FROM gcc:latest
COPY . /usr/src/work_dir
WORKDIR /usr/src/work_dir
RUN make re
# CMD ["./scheduler 3 3"]