import boto3
import decimal
import json
import time
from flask import Flask, render_template
from boto3.dynamodb.conditions import Key, Attr

app = Flask(__name__)
db = boto3.resource('dynamodb', region_name='eu-central-1')


class DecimalEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, decimal.Decimal):
            return str(o)
        return super(DecimalEncoder, self).default(o)


class DB:
    def retrieve_values(sensor, metric=None):
        table = db.Table('Measurements')
        response = table.query(
            KeyConditionExpression=Key('sensor').eq(sensor),
            ScanIndexForward=False,
            Limit=100
        )
        items = []
        for i in response[u'Items']:

            if metric is not None:
                i = i['message'][metric]
            else:
                i = i['message']

            items.append(i)
        return items

    def retrieve_value(sensor, metric=None):
        return DB.retrieve_values(sensor, metric)[0]


@app.route('/')
def landing_handler():
    sensors = {'air-sensor': ['temperature', 'humidity'], 'test': ['temperature', 'humidity','wind','wind_direction','rain'] }
    # sensors = {'test': ['temperature', 'humidity'], 'air-sensor': ['temperature', 'humidity']}

    return render_template('index.html', DB=DB, sensors=sensors)


@app.route('/<sensor>/')
def sensor(sensor):
    return json.dumps(DB.retrieve_values(sensor), cls=DecimalEncoder)


@app.route('/<sensor>/last')
def sensor_metric(sensor):
    return json.dumps(DB.retrieve_value(sensor), cls=DecimalEncoder)


if __name__ == '__main__':
    # app.run()
    app.run(host="192.168.1.104")
