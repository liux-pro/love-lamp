from http.server import BaseHTTPRequestHandler, HTTPServer
import json

from esp_idf_nvs_partition_gen.nvs_partition_gen import nvs_open, nvs_close, write_entry, check_size, Page


class BinaryDataHandler(BaseHTTPRequestHandler):
    # def end_headers(self):
    #     self.send_header('Access-Control-Allow-Origin', '*')
    #     BinaryDataHandler.end_headers(self)

    def do_OPTIONS(self):
        self.send_response(200, "ok")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'POST, OPTIONS')
        self.send_header("Access-Control-Allow-Headers", "X-Requested-With")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)

        try:
            data = json.loads(post_data)

            self.send_response(200)
            self.send_header('Content-type', 'application/octet-stream')
            self.send_header('Access-Control-Allow-Origin', '*')

            self.end_headers()
            nvs_obj = nvs_open(self.wfile, check_size("0x3000"), Page.VERSION2, is_encrypt=False, key=None)
            write_entry(nvs_obj, "storage", "namespace", "", "")
            for key, value in data.items():
                write_entry(nvs_obj, key, "data", "string", value)
            nvs_close(nvs_obj)

        except json.JSONDecodeError:
            self.send_response(400)
            self.end_headers()
            self.wfile.write(json.dumps({'error': 'Invalid JSON'}).encode())


def run_server(port=8080):
    server_address = ('', port)
    httpd = HTTPServer(server_address, BinaryDataHandler)
    print(f'Server is running on port {port}')
    httpd.serve_forever()


if __name__ == '__main__':
    run_server()
