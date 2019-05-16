#Average_filter for the data
#Note that it is only one input although the value is called inputs

def averaging_filter(inputs, stored_values, order):
    count = order
    final_result = 0
    k = 1.0/(1+order)

    if order == 0:
        return inputs

    while count >= 0:
        if count == 0:
            final_result = final_result + k*inputs
        final_result = final_result + k*stored_values[count]
        count-=1

    count = order-1
    while count >= 0:
        if count == 0:
            stored_values[count] = inputs
        else:
            stored_values[count] = stored_values[count-1]
        count-=1

    return final_result
