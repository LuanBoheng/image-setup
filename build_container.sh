sudo dockerd
sudo docker pull sellpy/python3-jupyter-sklearn
sudo docker volume create learning_index
sudo docker run -it -p 8888:8888 -v learning_index:/learning_index sellpy/python3-jupyter-sklearn:latest bash
