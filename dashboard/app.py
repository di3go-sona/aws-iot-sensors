from datetime import datetime, timedelta

import sys
import boto3
import decimal
import json
import time
from flask import Flask, render_template
from boto3.dynamodb.conditions import Key, Attr

app = Flask(__name__)
db = boto3.resource('dynamodb', region_name='eu-central-1')

# Utility class to parse dynamodb answers
class DecimalEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, decimal.Decimal):
            return str(o)
        return super(DecimalEncoder, self).default(o)

# Utility class to query the db
class DB:
    # Query db for the last 20 measures of a sensor
    def retrieve_values(sensor, metric=None):
        table = db.Table('Measurements')
        response = table.query(
            KeyConditionExpression=Key('sensor').eq(sensor),
            ScanIndexForward=False,
            Limit=20
        )
        items = []

        for i in response[u'Items']:
            # Transforming timestamp to readable hour and minutes
            # sorry for the awfullness of this line
            i['message']['timestamp'] = datetime.fromtimestamp(int(i['timestamp'][:-3])).strftime('%H:%M')

            if metric is not None:
                #if metric is set return only that value
                i = i['message'][metric]
            else:
                # otherwise return all metrics for this sensor
                i = i['message']

            items.append(i)
        return items

    # Query db for the last measures of a sensor
    def retrieve_value(sensor, metric=None):
        return DB.retrieve_values(sensor, metric)[0]

# Index Page
@app.route('/')
def landing_handler():
    # sensors = {'air-sensor': ['temperature', 'humidity'],
    #            'test': ['temperature', 'humidity','wind','wind_direction','rain'] }
    sensors = {'air-sensor': ['temperature', 'humidity'],
               'riot-test-1': ['temperature', 'humidity',],
               'riot-test-2': ['temperature', 'humidity',]  }

    # Render the default template with the help of jinja
    return render_template('index.html', DB=DB, sensors=sensors)

# JSON API for last 20 values
# used to update the page with jQuery
@app.route('/<sensor>/')
def sensor(sensor):
    return json.dumps(DB.retrieve_values(sensor), cls=DecimalEncoder)

# JSON API for last value
# used to update the page with jQuery
@app.route('/<sensor>/last')
def sensor_metric(sensor):
    return json.dumps(DB.retrieve_value(sensor), cls=DecimalEncoder)


if __name__ == '__main__':
    if len(sys.argv) >= 2:
        host = sys.argv[1]
    else:
        host = '127.0.0.1'
        

    app.run(host=host)
