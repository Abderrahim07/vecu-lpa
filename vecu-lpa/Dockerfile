FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    gcc make python3 python3-pip \
    libglib2.0-0 libgl1-mesa-glx \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --no-cache-dir \
    boto3 numpy awscli \
    flask flask-socketio \
    eventlet \
    opencv-python-headless

WORKDIR /vecu

COPY LPA_EQUI_VCodeGen_ert_rtw_last/LPA_EQUI_VCodeGen_ert_rtw/LPA_EQUI_VCodeGen.c ./firmware/
COPY LPA_EQUI_VCodeGen_ert_rtw_last/LPA_EQUI_VCodeGen_ert_rtw/LPA_EQUI_VCodeGen.h ./firmware/
COPY LPA_EQUI_VCodeGen_ert_rtw_last/LPA_EQUI_VCodeGen_ert_rtw/LPA_EQUI_VCodeGen_private.h ./firmware/
COPY LPA_EQUI_VCodeGen_ert_rtw_last/LPA_EQUI_VCodeGen_ert_rtw/LPA_EQUI_VCodeGen_types.h ./firmware/
COPY LPA_EQUI_VCodeGen_ert_rtw_last/LPA_EQUI_VCodeGen_ert_rtw/rtwtypes.h ./firmware/
COPY bsw/ ./firmware/
COPY scripts/ ./scripts/
COPY templates/ ./templates/

RUN gcc -O2 -Wall \
    firmware/bsw_main.c \
    firmware/bsw_os.c \
    firmware/bsw_com.c \
    firmware/bsw_mcu.c \
    firmware/bsw_nvm.c \
    firmware/LPA_EQUI_VCodeGen.c \
    -I firmware/ \
    -lm -lrt \
    -o firmware_bsw

RUN mkdir -p inputs/videos outputs

EXPOSE 5000
CMD ["python3", "scripts/app.py"]
