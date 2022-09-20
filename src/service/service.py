import os
from flask import Flask
import datetime
import subprocess
from flask import request
from flask_cors import CORS
from os.path import join
import logging

x = datetime.datetime.now()

# Initializing flask app
app = Flask(__name__)
CORS(app)

ROOT_PATH = os.path.dirname(os.path.realpath(__file__))


def remove_files(files):
    for f in files:
        os.remove(join(ROOT_PATH, f))


# Route for seeing a data
@app.route('/enc')
def enc():
    # exec binary file to get output
    patient_name = request.args.get('patient', default='*', type=str).strip()
    print(patient_name)
    logging.info(patient_name)
    cmd = f"{join(ROOT_PATH, 'bin', 'client_enc')} <<< " + patient_name

    subprocess.call(['bash', '-c', cmd])

    with open(join(ROOT_PATH, 'context.txt')) as f:
        context = f.read()

    with open(join(ROOT_PATH, 'enc_query.txt')) as f:
        enc_query = f.read()

    with open(join(ROOT_PATH, 'pubkey.txt')) as f:
        pubkey = f.read()

    data = {
        'context': context,
        'enc_query': enc_query,
        'pubkey': pubkey
    }
    # remove_files(['context.txt', 'enc_query.txt', 'pubkey.txt'])
    # return output to client
    return data


# Route for seeing a data
@app.route('/dec', methods=['POST'])
def dec():
    # exec binary file to get output
    data = request.get_json()
    with open(join(ROOT_PATH, 'enc_result.txt'), 'w') as f:
        f.write(data["enc_result"])

    cmd = f"{join(ROOT_PATH, 'bin', 'client_dec')} > {join(ROOT_PATH, 'log.txt')}"
    subprocess.call(['bash', '-c', cmd])

    str_result = open(join(ROOT_PATH, 'log.txt'), "r").read().split("Query result: ")[-1].split("\u0000")[0]
    print(str_result)
    remove_files(['log.txt', 'enc_result.txt', 'seckey.txt', 'context.txt', 'enc_query.txt', 'pubkey.txt'])
    # return output to client
    return {
        "ptx_result": str_result
    }


# Running app
if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0", port=9999)

