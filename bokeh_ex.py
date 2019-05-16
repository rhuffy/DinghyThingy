import numpy as np
import math

from bokeh.layouts import row, column
from bokeh.models import CustomJS, Slider
from bokeh.plotting import figure, output_file, show, ColumnDataSource
from bokeh.tile_providers import get_provider, Vendors

# lat = [
# 42.358204,
# 42.358208,
# 42.358208,
# 42.358215,
# 42.358227,
# 42.358231,
# 42.358238,
# 42.358246,
# 42.358246,
# 42.358242,
# 42.358238,
# 42.358242,
# 42.358238,
# 42.358242,
# 42.358238,
# 42.358231,
# 42.358223,
# 42.358212,
# 42.358204,
# 42.358196,
# 42.358181,
# 42.358166,
# 42.358158,
# 42.358158,
# 42.358154,
# 42.358158,
# 42.358154,
# 42.358154,
# 42.35815,
# 42.358147,
# 42.358147,
# 42.358143,
# 42.358139,
# 42.358139,
# 42.358139,
# 42.358139,
# 42.358139,
# 42.358139,
# 42.358135,
# 42.358131,
# 42.358128,
# 42.35812,
# 42.358112,
# 42.358089,
# 42.358086,
# 42.358082,
# 42.358082,
# 42.358078,
# 42.358078,
# 42.358078,
# 42.358074,
# 42.358074,
# 42.358074,
# 42.358063,
# 42.358051,
# 42.35804,
# 42.358028,
# 42.358009,
# 42.357994,
# 42.357986,
# 42.357986,
# 42.357998,
# 42.358009,
# 42.358009,
# 42.358013,
# 42.358017,
# 42.358021,
# 42.358028,
# 42.358036,
# 42.358051,
# 42.358067,
# 42.358082,
# 42.358086,
# 42.358089,
# 42.358097,
# 42.358105,
# 42.358109,
# 42.358124,
# 42.358128,
# 42.358131,
# 42.358131,
# 42.358139,
# 42.358143,
# 42.358143,
# 42.35815,
# ]
# lon = [
# -71.087616,
# -71.087608,
# -71.087608,
# -71.087601,
# -71.087608,
# -71.087624,
# -71.087639,
# -71.087654,
# -71.087669,
# -71.087669,
# -71.087669,
# -71.087677,
# -71.087685,
# -71.087692,
# -71.087708,
# -71.087723,
# -71.087715,
# -71.087723,
# -71.08773,
# -71.087738,
# -71.087746,
# -71.087753,
# -71.087769,
# -71.087784,
# -71.087784,
# -71.087791,
# -71.087799,
# -71.087807,
# -71.087814,
# -71.087822,
# -71.087822,
# -71.08783,
# -71.08783,
# -71.08783,
# -71.087837,
# -71.087837,
# -71.087837,
# -71.087837,
# -71.087845,
# -71.08786,
# -71.087868,
# -71.087883,
# -71.087898,
# -71.087975,
# -71.08799,
# -71.08799,
# -71.08799,
# -71.087997,
# -71.088005,
# -71.088005,
# -71.088013,
# -71.088013,
# -71.08802,
# -71.088043,
# -71.088066,
# -71.088089,
# -71.088112,
# -71.08815,
# -71.088173,
# -71.088188,
# -71.088211,
# -71.088226,
# -71.088226,
# -71.088226,
# -71.088226,
# -71.088219,
# -71.088219,
# -71.088211,
# -71.088203,
# -71.088188,
# -71.088158,
# -71.088135,
# -71.08812,
# -71.088104,
# -71.088081,
# -71.088074,
# -71.088081,
# -71.088058,
# -71.088043,
# -71.088043,
# -71.088043,
# -71.088036,
# -71.088028,
# -71.088028,
# -71.08802,
# ]

RADIUS = 6378137.0 # in meters on the equator

def lat2y(a):
  return math.log(math.tan(math.pi / 4 + math.radians(a) / 2)) * RADIUS

def lon2x(a):
  return math.radians(a) * RADIUS


def gen_animated_plot(lat, lon):
    """
    Creates an interactive bokeh plot.
    :param lat  array of latitude points
    :param lon  array of longitude points
    """
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

    output_file("boat_viewer.html", title="Boat Viewer")

    show(layout)
