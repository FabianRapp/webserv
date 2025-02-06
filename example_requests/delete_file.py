HOST = "has_delete.com"

TYPE = "DELETE"
URI = "/here_is_delete_allowed/delete_me.txt"

http_request = """\
{type} {uri} HTTP/1.1\r
Host: {host}\r
User-Agent: CustomClient/1.0\r
Connection: close\r
\r
""".format(type=TYPE, uri=URI, host=HOST)
