from flask import Flask
import datetime
import subprocess
from flask import request
from flask_cors import CORS
import os
from os.path import join

ROOT_PATH = os.path.dirname(os.path.realpath(__file__))

x = datetime.datetime.now()

# Initializing flask app
app = Flask(__name__)
CORS(app)


def remove_files(files):
    for f in files:
        os.remove(join(ROOT_PATH, f))


# Route for seeing a data
@app.route('/search', methods=['POST'])
def search():
    data = request.get_json()
    # context, enc_query and pubkey must be in the same dir with server.py
    with open(join(ROOT_PATH, 'context.txt'), 'w') as f:
        f.write(data["context"])
    with open(join(ROOT_PATH, 'enc_query.txt'), 'w') as f:
        f.write(data["enc_query"])
    with open(join(ROOT_PATH, 'pubkey.txt'), 'w') as f:
        f.write(data["pubkey"])

    # exec binary file to get output
    cmd = f"{join(ROOT_PATH, 'bin', 'server')} db_filename={join(ROOT_PATH, 'bin', 'dataset.csv')}"
    subprocess.call(['bash', '-c', cmd])

    with open(join(ROOT_PATH, 'enc_result.txt'), 'r') as f:
        enc_result = f.read()

    remove_files(['context.txt', 'enc_query.txt', 'pubkey.txt', 'enc_result.txt'])
    # return output to client
    return {
        'enc_result': enc_result
    }


# Running app
if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0", port=5000)
