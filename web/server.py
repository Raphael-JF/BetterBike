import http.server, ssl

server = http.server.HTTPServer(('127.0.0.1', 4443),
                               http.server.SimpleHTTPRequestHandler)

server.socket = ssl.wrap_socket(server.socket,
                                keyfile="key.pem",
                                certfile="cert.pem",
                                server_side=True)

server.serve_forever()