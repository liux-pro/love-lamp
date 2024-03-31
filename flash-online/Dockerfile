# 使用官方的 Nginx 镜像作为基础镜像
FROM python:alpine

# 设置工作目录
WORKDIR /app

# 将本地的静态文件复制到 Nginx 的默认 HTML 目录下
COPY dist /usr/share/nginx/html

# 将本地的 Nginx 配置文件替换为自定义的配置文件
COPY server/nginx.conf /etc/nginx/nginx.conf

COPY server/main.py /app/main.py
COPY server/requirements.txt /app/requirements.txt

# 安装 Nginx 和 Python3
RUN apk update && apk add nginx

RUN pip install -r /app/requirements.txt


# 暴露 8080 端口
EXPOSE 8080

# 启动 Nginx
CMD ["nginx", "-g", "daemon off;"]
