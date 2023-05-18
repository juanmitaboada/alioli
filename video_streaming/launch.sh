#!/bin/bash

if [[ -z "$2" ]] ; then
    video="/dev/video2"
else
    video="$2"
fi

# == NEW ==
case $1 in
    1)
        rtspServer=127.0.0.1:rtsp://127.0.0.1:8554/alioli ./rtsp-simple-server
        break
        ;;
    2)
        ffmpeg -f v4l2 -framerate 90 -re -stream_loop -1 -video_size 640x320 -input_format mjpeg -i $video -c copy -f rtsp rtsp://localhost:8554/alioli
        break
        ;;
    *)
        echo "Usage: $0 [1|2] <video device: only on 2>"
esac

# == OLD ==
# rtspServer=127.0.0.1:rtsp://127.0.0.1:8554/webCamStream ./rtsp-simple-server
# sudo ffmpeg -f v4l2 -framerate 24 -video_size 480x480 -i /dev/video0 -f rtsp -rtsp_transport tcp rtsp://127.0.0.1:8554/webCamStream
