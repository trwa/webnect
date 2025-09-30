FROM fedora:44
LABEL authors="Lorenzo"

RUN dnf install -y clang libfreenect libfreenect-devel opencv opencv-devel

ENTRYPOINT ["top", "-b"]