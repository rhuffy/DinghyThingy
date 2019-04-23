# -*- coding: utf-8 -*-
"""
Created on Thu Apr 18 13:43:06 2019

@author: mayan
"""

import sqlite3
from datetime import datetime

#_________________________________________________________________________________________________
# Request handling
#-------------------------------------------------------------------------------------------------

db = "__HOME__/final_project/dinghy_thingy.db" # just come up with name of database

def request_handler(request):
    
    #make sure categories is consistent across requests
    
    if request['method']=='POST':
        if 'categories' not in request['form'] or 'data' not in request['form']:
            return "Please submit data and a list of categories"
        else:
            #try:
                
                str_categories = request['form']['categories']
                categories = parse_csv_categories(str_categories)
                create_database(categories)
            
                
                data = request['form']['data']
                list_of_data_tuples = parse_csv_data(data, categories)
                
                for data_tuple in list_of_data_tuples:
                    if data_tuple != None:
                        insert_into_database(data_tuple)
                
                return "Data entered successfully! Please do GET request to access information"
            #except:
             #   return  "Error, please try again!"
            
    elif 'boatnum' not in request['values']:
        return lookup_database()
    else:
        try:
            
            boatnum = int(request['values']['boatnum'])
            
            return get_boat_data(boatnum)
        except:
            return  "Both lat and lon must be valid numbers."

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
    if len(str_data_list) != len(categories):
        print("Error: data not same length as categories")
    else:
        for i in range(len(str_data_list)):
            str_data = str_data_list[i]
            if str_data != None:
                info = categories[i]
                if info["datatype"] == datetime:
                    data[info["category"]] = datetime.strptime(str_data.strip(), "%Y-%m-%dT%H:%M:%S.%f")
                else:
                    data[info["category"]] = info["datatype"](str_data)
        return tuplefy(data)

def parse_csv_data(data, categories): #data string of info
                                      #categories format is list of dictionaries of the format {"category":str, "datatype":datatype}
    if "\n" in data:
        data_lines = data.split("\n")
    elif "\\n" in data:
        data_lines = data.split("\\n")
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
    
    executed = c.execute('''SELECT * FROM dinghy_thingy;''')
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

def get_boat_data(boatnum):
    conn = sqlite3.connect(db)
    c = conn.cursor()
    
    executed = c.execute('''SELECT * FROM dinghy_thingy WHERE boatnum = ? ORDER BY time ASC;''',(boatnum,) )
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
 


