# -*- coding: utf-8 -*-
"""
Created on Tue Feb 26 09:19:30 2019

@author: mayan
"""

import sqlite3
import datetime
import requests

API_KEY = "3404c75b6075824fcec1f084abdfe535"

def get_current_data(category):
    if category in {'temp', 'visibility'}:
        r = requests.get("""http://api.openweathermap.org/data/2.5/weather?zip=02139&units=imperial&APPID=%s"""%API_KEY)
        response = r.json()
        
        if category == 'temp':
            try:
                temp = response["main"]['temp']
                return "The temp is %d deg F"%temp
            except:
                return "Error getting temperature"
        elif category == 'visibility':
            try:
                vis = response["weather"][0]['main']
                vis_dict = {"Clouds":"Cloudy", "Rain":"Rainy", "Snow":"Snowy","Drizzle":"Drizzley"}
                if vis in vis_dict:
                    vis = vis_dict[vis]
                return "The visibility is %s"%vis
            except:
                return "Error getting visibility"
        
    elif category in {'date', 'time'}:
        currentDT = datetime.datetime.now()
        
        if category == 'date':
            return "The date is %d/%d/%d" % (currentDT.month, currentDT.day, currentDT.year)
        
        elif category == 'time':
            hour = currentDT.hour
            minute = currentDT.minute
            
            if hour == 0:
                hour = 12
                am_pm = "AM"
            elif hour < 12:
                am_pm = "AM"
            else:
                hour -= 12
                am_pm = "PM"
                
            if minute < 10:
                minute_str = "0" + str(minute)
            else:
                minute_str = str(minute)
                
            return "The time is %d:%s %s" % (hour, minute_str, am_pm)
            
            
def request_handler(request):    
    if request['method']=='POST':
        return "POST requests not allowed."
    elif 'category' not in request['values']:
        return "You must enter a category."
    else:
        try:
            category = request['values']['category']
            
            if category not in {'temp', 'time', 'date', 'visibility', 'clear' }:
                return "Not a valid category"
            else:
                visits_db = '__HOME__/ex04/weather.db'
                
                conn = sqlite3.connect(visits_db)  # connect to that database (will create if it doesn't already exist)
                c = conn.cursor()  # make cursor into database (allows us to execute commands)
                outs = ""
                c.execute('''CREATE TABLE IF NOT EXISTS weather_table (timing timestamp, data text);''') # run a CREATE TABLE command
                if category == 'clear':
                    c.execute('''DELETE FROM weather_table;''')
                    conn.commit() # commit commands
                    conn.close() # close connection to database
                    return ""
                else:
                    data = get_current_data(category)
                    c.execute('''INSERT into weather_table VALUES (?,?);''', (datetime.datetime.now(), data))
                    things = c.execute('''SELECT data FROM weather_table ORDER BY timing DESC;''').fetchall()
                    outs = ""
                    for x in things:
                        outs+=x[0]+"\n"
                    conn.commit() # commit commands
                    conn.close() # close connection to database
                    return outs
        except:
            return  "Somethang aint right bitch"
    