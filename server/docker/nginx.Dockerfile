FROM nginx

WORKDIR /etc/nginx

COPY ../nginx/nginx.conf /etc/nginx/nginx.conf
COPY  ../test /etc/nginx/static/