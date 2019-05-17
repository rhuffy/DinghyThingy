# -*- coding: utf-8 -*-
"""
Created on Thu Apr 18 13:43:06 2019

@author: mayan
"""

import sqlite3
from datetime import datetime
import pandas as pd
from bokeh.plotting import figure, output_file, save, show
import requests
import math
from bokeh.layouts import row, column
from bokeh.models import CustomJS, Slider
from bokeh.plotting import ColumnDataSource
from bokeh.tile_providers import get_provider, Vendors

#_________________________________________________________________________________________________
# Request handling
#-------------------------------------------------------------------------------------------------

home = "__HOME__/final_project/" #path for server testing
#home = "" # path for on-computer testing
db = home+"dinghy_thingy.db" 

#example post
p = {'form': {'categories': 'int boatnum, datetime time, float lat, float lon, float x_accel, float y_accel, float z_accel','data': '22, 2019-04-24T13:29:13.5, 42.357, -71.091, .01, .01, 59.04\n227,  2019-04-24T13:32:16.5, 42.355, -71.094, .1, .1, 589.09\n22, 2019-04-24T13:32:15.5, 42.356, -71.094, .13, .13, 59.04\n22, 2019-04-24T13:35:16.8, 42.353, -71.100, .3, .3, 59.04'},'method': 'POST'}
#example get
g = {'method':'GET', 'values':{'date':'2019-04-24', 'boatnum':22}}
#example get
g2 = {'method':'GET', 'values':{'date':'2019-04-24'}}
#example get
g3={'method': 'GET', 'values': {}}
def request_handler(request):
    
    #returns 1 if successful, 0 if not
    
    if request['method']=='POST':
        if 'categories' not in request['form'] or 'data' not in request['form']:
            return 0
        else:
            #try:
                #get categories from post
                str_categories = request['form']['categories']
                categories = parse_csv_categories(str_categories)
                create_database(categories)
            
                #get data from post
                data = request['form']['data']
                list_of_data_tuples = parse_csv_data(data, categories)
                
                for data_tuple in list_of_data_tuples:
                    if data_tuple != None:
                        insert_into_database(data_tuple)
                
                return 1 #data was added to database succesfully
            #except:
             #   return  0 #there was an error
            
    elif 'boatnum' not in request['values'] and 'date' not in request['values']:
        return response_no_param()
    elif 'boatnum' in request['values'] and 'date' in request['values']:
        
        #try:
            boatnum = int(request['values']['boatnum'])
            date = datetime.strptime(request['values']['date'].strip(), "%Y-%m-%d")
            return single_boat_response(date, boatnum)
        #except:
         #   return  "boatnum must be valid numbers and date must be valid date in format YYYY-MM-DD."
        
    elif 'boatnum' in request['values']:
        
        try:
            boatnum = int(request['values']['boatnum'])
            return print_boat_data(boatnum)
        except:
            return  "boatnum must be valid numbers."
        
    elif 'date' in request['values']:
        
        #try:
            date = datetime.strptime(request['values']['date'].strip(), "%Y-%m-%d")
            return response(date)
        #except:
         #   return  "date must be valid date in format YYYY-MM-DD."
        
    else:
        return "Invalid request"

#_________________________________________________________________________________________________
# Functions for parsing data once it's been extracted from the request
#-------------------------------------------------------------------------------------------------

'''
//Example category data://

int boatnum, int time, float lat, float lon, float x_accel, float y_accel, float z_accel

//Example data://

2234567, 1098765432, 435678.9564, 567.345678, 65789.4321, 45678.98765, 546789.09654
45678, 23456789, 34.567890, 1234.456789, 20987654.2, 56789.65432, 56789.09876
'''

def parse_csv_categories(line):
    data_list = line.split(",")
    categories = []
    for elem in data_list:
        if elem != None:
            type_and_data = elem.split();
            datatype = eval(type_and_data[0])
            data = type_and_data[1]
            categories.append({"category":data, "datatype":datatype})
    return categories #list of dictionaries of the format {"category":str, "datatype":datatype}

def tuplefy(data): #assumes data is dictionary where category : data (category is a string, data is its natural datatype)
    dlist = []
    for elem in data:
        dlist.append(data[elem])
    return tuple(dlist)

def parse_csv_data_line(line, categories):
    str_data_list = line.split(",")
    data = {}
#    if len(str_data_list) != len(categories):
#        print("Error: data not same length as categories")
#    else:
    if len(str_data_list) == len(categories):
        for i in range(len(str_data_list)):
            str_data = str_data_list[i]
            if str_data != None:
                info = categories[i]
                if info["datatype"] == datetime:
                    if "." in str_data:
                        data[info["category"]] = datetime.strptime(str_data.strip(), "%Y-%m-%dT%H:%M:%S.%f")
                    else:
                        data[info["category"]] = datetime.strptime(str_data.strip(), "%Y-%m-%dT%H:%M:%S")
                else:
                    data[info["category"]] = info["datatype"](str_data)
        return tuplefy(data)

def parse_csv_data(data, categories): #data string of info
                                      #categories format is list of dictionaries of the format {"category":str, "datatype":datatype}
    if "\n" in data:
        data_lines = data.split("\n")
    elif "\\n" in data:
        data_lines = data.split("\\n")
    else:
        return [parse_csv_data_line(data, categories)]
    data = []
    for line in data_lines:
        data.append(parse_csv_data_line(line, categories))
    return data

#_________________________________________________________________________________________________
# Functions that take our data and create strings for my convenience for database interaction
#-------------------------------------------------------------------------------------------------
    
def string_rep_categories(categories):
    dtype_dict = {str:"text", int:"int", float:"float", datetime:"timing"}
    string = "("
    for d in categories:
        name = d["category"]
        dtype = dtype_dict[d["datatype"]]
        string += name + " " + dtype+ ","
    string = string[:-1]
    string+=")"
    return string

def create_insert_string(data):
    string = "("
    for d in data:
        string += "?,"
    string = string[:-1]
    string+=")"
    return string

#_________________________________________________________________________________________________
# Database interaction functions
#-------------------------------------------------------------------------------------------------
 
def create_database(categories):
    conn = sqlite3.connect(db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS dinghy_thingy '''+string_rep_categories(categories)+''';''') # run a CREATE TABLE command
    conn.commit() # commit commands
    conn.close() # close connection to database

def insert_into_database(data):
    conn = sqlite3.connect(db)
    c = conn.cursor()
    c.execute('''INSERT into dinghy_thingy VALUES '''+ create_insert_string(data) +''';''',data)
    conn.commit()
    conn.close()

def lookup_database():
    conn = sqlite3.connect(db)
    c = conn.cursor()
    
    executed = c.execute('''SELECT * FROM dinghy_thingy ORDER BY time DESC;''')
    things = executed.fetchall()
    names = [description[0] for description in executed.description]
    
    df = pd.DataFrame(data=things, columns=names)
    return str(df)

def print_boat_data(boatnum):
    conn = sqlite3.connect(db)
    c = conn.cursor()
    
    executed = c.execute('''SELECT * FROM dinghy_thingy WHERE boatnum = ? ORDER BY time DESC;''',(boatnum,) )
    things = executed.fetchall()
    names = [description[0] for description in executed.description]
    
    printout = ""
    for cat in names:
        printout += str(cat) + "\t\t\t"
    printout+="\n"
    for row in things:
        for info in row:
            printout += str(info) + "\t\t\t"
        printout += "\n"
    conn.commit()
    conn.close()
    return printout

def print_date_data(date):
    conn = sqlite3.connect(db)
    c = conn.cursor()
    
    beginning_of_day = date;
    end_of_day = datetime(date.year,date.month,date.day,23,59,59,999999)
    
    executed = c.execute('''SELECT * FROM dinghy_thingy WHERE time > ? AND time < ? ORDER BY time DESC;''',(beginning_of_day, end_of_day,))
    things = executed.fetchall()
    names = [description[0] for description in executed.description]
    
    printout = ""
    for cat in names:
        printout += str(cat) + "\t\t\t"
    printout+="\n"
    for row in things:
        for info in row:
            printout += str(info) + "\t\t\t"
        printout += "\n"
    conn.commit()
    conn.close()
    return printout

def print_dateandboat_data(date, boatnum):
    
    conn = sqlite3.connect(db)
    c = conn.cursor()
    
    beginning_of_day = date;
    end_of_day = datetime(date.year,date.month,date.day,23,59,59,999999)
    
    executed = c.execute('''SELECT * FROM dinghy_thingy WHERE boatnum = ? AND time > ? AND time < ? ORDER BY time DESC;''',(boatnum, beginning_of_day, end_of_day,))
    things = executed.fetchall()
    names = [description[0] for description in executed.description]
    
    printout = ""
    for cat in names:
        printout += str(cat) + "\t\t\t"
    printout+="\n"
    for row in things:
        for info in row:
            printout += str(info) + "\t\t\t"
        printout += "\n"
    conn.commit()
    conn.close()
    return printout
#_________________________________________________________________________________________________
# Google Maps API preparation and interaction functions
#-------------------------------------------------------------------------------------------------
'''
//Example category data://

int boatnum, int time, float lat, float lon, float x_accel, float y_accel, float z_accel

//Example data://

2234567, 1098765432, 435678.9564, 567.345678, 65789.4321, 45678.98765, 546789.09654
45678, 23456789, 34.567890, 1234.456789, 20987654.2, 56789.65432, 56789.09876
'''

def get_dateandboat_data(date, boatnum):
    
    conn = sqlite3.connect(db)
    c = conn.cursor()
    
    beginning_of_day = date;
    end_of_day = datetime(date.year,date.month,date.day,23,59,59,999999)
    
    executed = c.execute('''SELECT * FROM dinghy_thingy WHERE boatnum = ? AND time > ? AND time < ? ORDER BY time ASC;''',(boatnum, beginning_of_day, end_of_day,))
    things = executed.fetchall()
    names = [description[0] for description in executed.description]
    
    conn.commit()
    conn.close()
    
    df = pd.DataFrame(data=things, columns=names)
    return df

def get_boat_data(boatnum):
    
    conn = sqlite3.connect(db)
    c = conn.cursor()
    
    executed = c.execute('''SELECT * FROM dinghy_thingy WHERE boatnum = ? ORDER BY time ASC;''',(boatnum,))
    things = executed.fetchall()
    names = [description[0] for description in executed.description]
    
    conn.commit()
    conn.close()
    
    df = pd.DataFrame(data=things, columns=names)
    return df

def get_origin(df):
    return str(df[0:1]["lat"].iloc[0])+","+str(df[0:1]["lon"].iloc[0])

def get_destination(df):
    return str(df[-1:]["lat"].iloc[0])+","+str(df[-1:]["lon"].iloc[0])

def get_waypoints(df):
    waypoints=""
    rows = df.shape[0]
    for i in range(1,rows-1):
        waypoints+=str(df[i:i+1]["lat"].iloc[0])+","+str(df[i:i+1]["lon"].iloc[0])+"%7C"
    return waypoints[:-3]

def get_locations(df):
    locations=""
    rows = df.shape[0]
    for i in range(0,rows):
        locations+=str(df[i:i+1]["lat"].iloc[0])+","+str(df[i:i+1]["lon"].iloc[0])+"%7C"
    return locations[:-3]

def get_markers(df):
    return "size:tiny%7Ccolor:green%7C" + get_origin(df) + "&markers=size:tiny%7Ccolor:black%7C" + get_waypoints(df) + "&markers=size:tiny%7Ccolor:red%7C" + get_destination(df)

def get_path(string_of_locations):
    return "color:black%7Cweight:1%7C" + string_of_locations

def embed_map(df):
    
    MY_API_KEY = "AIzaSyB4-QmxO-jJnljJD1dNpnZ85AcgbyCMjyw"
    string_of_locations = get_locations(df)
    markers = get_markers(df)
    path = get_path(string_of_locations)
    request_string = "https://maps.googleapis.com/maps/api/staticmap?key={}&size=500x500&markers={}&path={}".format(MY_API_KEY,markers,path)
    return request_string

RADIUS = 6378137.0 # in meters on the equator

def lat2y(a):
  return math.log(math.tan(math.pi / 4 + math.radians(a) / 2)) * RADIUS

def lon2x(a):
  return math.radians(a) * RADIUS

#animate the path of the boat using bokeh
def gen_animated_plot(df, boatnum):
    """
    Creates an interactive bokeh plot.
    :param df  dataframe of data
    :param boatnum  boatnumber associated with this plot
    """
    
    lat = df['lat'].values
    lon = df['lon'].values
    
    data = {
        'x': [lon2x(i) for i in lon],
        'y': [lat2y(i) for i in lat]
    }

    empty_data = {
        'x': [],
        'y': []
    }

    source_visible = ColumnDataSource(data=empty_data)
    source_available = ColumnDataSource(data=data)

    # plot = figure(plot_width=400, plot_height=400)
    plot = figure(x_range = (min(data['x']), max(data['x'])),
                  y_range=(min(data['y']), max(data['y'])),
                  x_axis_type="linear", y_axis_type="linear",
                  plot_width=400, plot_height=400)
    plot.add_tile(get_provider(Vendors.CARTODBPOSITRON))

    # plot = figure(y_range=(-10, 10), plot_width=400, plot_height=400)

    plot.line('x', 'y', source=source_visible, line_width=3, line_alpha=0.6)
    plot.xaxis.major_tick_line_color = None  # turn off x-axis major ticks
    plot.xaxis.minor_tick_line_color = None  # turn off x-axis minor ticks

    plot.yaxis.major_tick_line_color = None  # turn off y-axis major ticks
    plot.yaxis.minor_tick_line_color = None
    plot.xaxis.major_label_text_font_size = '0pt'  # turn off x-axis tick labels
    plot.yaxis.major_label_text_font_size = '0pt'

    callback = CustomJS(args=dict(source_visible=source_visible,
                                  source_available=source_available), code="""
        var visible_data = source_visible.data;
        var available_data = source_available.data;
        var time = time.value;

        var x_avail = available_data['x']
        var y_avail = available_data['y']

        visible_data['x'] = []
        visible_data['y'] = []

        for (var i = 0; i < time; i++) {
            visible_data['x'].push(x_avail[i]);
            visible_data['y'].push(y_avail[i]);
        }
        source_visible.change.emit();
    """)

    time_slider = Slider(start=1, end=len(data['x']), value=1, step=1,
                        title=None, tooltips=False, callback=callback)
    callback.args["time"] = time_slider

    layout = row(
        plot,
        column(time_slider),
    )

    output_file(home+"animation_"+str(boatnum)+".html", title="Boat Viewer")

    save(layout)
    
    return "animation_"+str(boatnum)+".html"

#plot the heel of the boat using bokeh
def plot_heel(df,boatnum):
    df['hour'] = df['time'].apply(lambda x: int(x[11:13]) + int(x[14:16])/60 + int(x[17:19])/3600)
    x = df['hour'].values
    #y_accel will give a decimal value from 0 to 1 showing how tilted the device is on a scale of 0 to 90 degrees
    y = (df['y_accel'] * 100).values
    # output to static HTML file
    filename = "heel"+str(boatnum)+".html"
    output_file(home+filename)
    
    # create a new plot
    p = figure(
       y_range=[-100, 100], title="heel of boat over time",
       x_axis_label='24hr time', y_axis_label='percent tilt', width=480, height=480
    )

    # add some renderers
    p.circle(x, y, fill_color="black", line_color="black", size=6)
    p.line(x, y, line_color="black", line_dash="4 4")
    
    # show the results
    save(p)
    return filename

def single_boat_response(date, boatnum):
    df = get_dateandboat_data(date, boatnum)
    request_string = embed_map(df)
    img = plot_heel(df, boatnum)
    animation = gen_animated_plot(df, boatnum)
    #return '''<!DOCTYPE html>\n<html>\n\t<head>\n\t\t<meta charset="utf-8">\n\t\t<meta name="viewport" content="width=device-width">\n\t\t<title>Dinghy Thingy</title>\n\t</head>\n\t<body>\n\t\t<p><b>Path of the boat over time:</b></p>\n\t\t<img src={} alt="Map of boat path" >\n\t\t<p> Green is where is started, red is where it ended; </p>\n\t\t<p><b>Heel of the boat over time:</b></p>\n\t\t<img src={} alt="Plot of heel of boat" >\n\n\t</body>\n</html>'''.format(request_string, img)
    return '''<!DOCTYPE html>\n<html>\n\t<head>\n\t\t<meta charset="utf-8">\n\t\t<meta name="viewport" content="width=device-width">\n\t\t<title>Dinghy Thingy</title>\n\t</head>\n\t<body>\n\t\t<p><b>Path of the boat over time:</b></p>\n\t\t<img src={} alt="Map of boat path" >\n\t\t<p> Green is where is started, red is where it ended; </p>\n\t\t<br>\n\t\t<p> Animation of the path of the boat (move the slider to see the animation): </p>\n\t\t<iframe src={} height="500" width="800"></iframe>\n\t\t<p><b>Heel of the boat over time:</b></p>\n\t\t<iframe src={} style="border:none;" height="500" width="500"></iframe>\n\n\t</body>\n</html>'''.format(request_string, animation, img)

#same as set_up_webpage_without_date except does it from specified date only
def set_up_webpage(date, boatnum):
    df = get_dateandboat_data(date, boatnum)
    #preapre boatnum link
    request_string = embed_map(df)
    img = plot_heel(df, boatnum)
    animation = gen_animated_plot(df, boatnum)
    
    f=open(home+"boat.html", "r")
    contents = f.read()
    #print(contents)
    #print("______________")
    new_contents = contents.replace("{googlemaps}",request_string)
    new_contents = new_contents.replace("{heel}",img)
    new_contents = new_contents.replace("{animation}", animation)
    new_contents = new_contents.replace("{home}", home)
    #print(new_contents)
    boat_file = "boat"+str(boatnum)+".html"
    f= open(home+boat_file,"w+")
    f.write(new_contents)
    
    return "<a href=\""+ boat_file +"\"> Boat "+str(boatnum)+"</a>"

#calls function to get boat data from database as dataframe;
#calls other functions to turn that data into visualizations
#reads template html file, replaces text in template with visualizations,
#   writes modified content to new html file
def set_up_webpage_without_date(boatnum):
    df = get_boat_data(boatnum)
    #preapre boatnum link
    request_string = embed_map(df)
    img = plot_heel(df, boatnum)
    animation = gen_animated_plot(df, boatnum)
    
    f=open(home+"boat.html", "r")
    contents = f.read()
    new_contents = contents.replace("{googlemaps}",request_string)
    new_contents = new_contents.replace("{heel}",img)
    new_contents = new_contents.replace("{animation}", animation)
    new_contents = new_contents.replace("{home}", home)
    #print(new_contents)
    boat_file = "boat"+str(boatnum)+".html"
    f= open(home+boat_file,"w+")
    f.write(new_contents)
    
    return "<a href=\""+ boat_file +"\"> Boat "+str(boatnum)+"</a>"

#gets weather data from api
def get_weather():
    WEATHER_API_KEY = "3404c75b6075824fcec1f084abdfe535"

    r = requests.get("""http://api.openweathermap.org/data/2.5/weather?zip=02139&units=imperial&APPID=%s"""%WEATHER_API_KEY)
    response = r.json()
    temp = 'Temperature is '+str(response['main']['temp']) + ' degrees fahrenheit'
    wind = 'Speed of wind is ' + str(response['wind']['speed'])+' miles/hour'
    
    if response['weather'][0]['main'] == 'Clouds':
    		gif = '<iframe src="https://giphy.com/embed/pjw5mc8Ze2mH5m5yZ6" width="320" height="240" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/gif-this-pjw5mc8Ze2mH5m5yZ6"></a></p>'
    else if response['weather'][0]['main'] == 'Rain':
    		gif = '<iframe src="https://giphy.com/embed/xUPGcAn10RE7mK84SY" width="480" height="480" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/xUPGcAn10RE7mK84SY">via GIPHY</a></p>'
    else:
    		gif = '<iframe src="https://giphy.com/embed/lI8YNZc734UH6" width="320" height="320" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/sunny-lI8YNZc734UH6"></a></p>'
    return temp, wind, gif
    
#does same as response_no_param except only for data on specified date
def response(date):
    
    conn = sqlite3.connect(db)
    c = conn.cursor()
    
    beginning_of_day = date;
    end_of_day = datetime(date.year,date.month,date.day,23,59,59,999999)
    
    executed = c.execute('''SELECT boatnum FROM dinghy_thingy WHERE time > ? AND time < ? ORDER BY time ASC;''',(beginning_of_day, end_of_day,))
    things = executed.fetchall()
    conn.commit()
    conn.close()
    
    temp, wind, gif = get_weather()
    
    #get all links
    links = ""
    boatnums = set()
    for tup in things:
       if tup[0] not in boatnums:
           boatnums.add(tup[0])
           links +=  set_up_webpage(date, tup[0])+"\n    "
    links = links[:-5]
    #write new html code
    f=open(home+"template_index.html", "r")
    if f.mode == 'r':
      contents =f.read()
      new_contents = contents.replace("{link}", links)
      new_contents = new_contents.replace("{home}", home)
      new_contents = new_contents.replace("{temp}", temp)
      new_contents = new_contents.replace("{wind}", wind)
      new_contents = new_contents.replace("{gif}", gif)
      f2=open(home+"ui.html", "w+")
      f2.write(new_contents)
      return new_contents
  
# gets all boatnumbers from database,
# creates new webpage (by calling function) for each boat
# takes response from that function (aka link to boat html page) and replaces
#   blank text in template to get links to boat pages on website      
def response_no_param():
    
    conn = sqlite3.connect(db)
    c = conn.cursor()
    
    executed = c.execute('''SELECT boatnum FROM dinghy_thingy ORDER BY time ASC;''')
    things = executed.fetchall()
    conn.commit()
    conn.close()
    
    temp, wind, gif = get_weather()
    
    
    #get all links
    links = ""
    boatnums = set()
    for tup in things:
       if tup[0] not in boatnums:
           boatnums.add(tup[0])
           links +=  set_up_webpage_without_date(tup[0])+"\n    "
    links = links[:-5]
    #write new html code
    f=open(home+"template_index.html", "r")
    if f.mode == 'r':
      contents =f.read()
      new_contents = contents.replace("{link}", links)
      new_contents = new_contents.replace("{home}", home)
      new_contents = new_contents.replace("{temp}", temp)
      new_contents = new_contents.replace("{wind}", wind)
      new_contents = new_contents.replace("{gif}", gif)
      f2=open(home+"index.html", "w+")
      f2.write(new_contents)
      return new_contents
      
