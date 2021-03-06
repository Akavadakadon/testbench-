import random
import string
import os

USED_HOST = "192.168.44.101"
USED_URL = "https://" + USED_HOST + ":6121"
headers = t = string.Template('''HTTP/1.1 200 OK
Content-Length: $size
X-Original-Url: $url/$filename

''')


def _generate_random_string(size):
    return ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(size))


def generate_response_with_size(size):
    headers_str = headers.substitute(size=size, url=USED_URL, filename=build_filename(size))
    curr_size = size - len(headers_str)
    #if curr_size < 0:
    #    return None
    body = _generate_random_string(curr_size)
    return headers_str + body


def generate_request_with_size(size):
    return _generate_random_string(size)


def build_filename(size):
    return "resp_" + str(size) + "_b.txt"


def generate_responses():
    for i in range(0, 10):
        s = 2 << i
        filename = build_filename(s)
        str_res = generate_response_with_size(s)

        if str_res is None:
            continue

        directory = os.path.dirname(__file__)
        path_to_file = os.path.join(directory, "./../res/" + filename)
        text_file = open(path_to_file, "w")
        text_file.write(str_res)
        text_file.close()


if __name__ == "__main__":
    generate_responses()
