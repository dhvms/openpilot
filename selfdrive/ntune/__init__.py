import os
import json

CONF_PATH = '/data/ntune/'
GROUP_FILE = CONF_PATH + 'group.json'
CTRL_FILE = CONF_PATH + 'ctrl.json'
LAT_FILE = CONF_PATH + 'lat.json'
LON_FILE = CONF_PATH + 'lon.json'
PATH_FILE = CONF_PATH + 'path.json'
CAMERA_FILE = CONF_PATH + 'camera.json'

CONF_LAT_INDI_FILE = CONF_PATH + 'lat_indi.json'
CONF_LAT_TORQUE_FILE = CONF_PATH + 'lat_torque_v4.json'
CONF_SCC_FILE = CONF_PATH + 'scc_v3.json'
CONF_COMMON_FILE = CONF_PATH + 'common.json'

def read_config_file(file_path):
    try:
        with open(file_path, 'r') as f:
            return json.load(f)
    except IOError:
        return {}

def ntune_common_get():
    group = {}
    ctrl = {}
    lat = {}
    lon = {}
    path = {}
    camera = {}

    try:
        with open(GROUP_FILE) as f:
            group = json.load(f)
    except IOError:
        pass

    try:
        with open(CTRL_FILE) as f:
            ctrl = json.load(f)
    except IOError:
        pass

    try:
        with open(LAT_FILE) as f:
            lat = json.load(f)
    except IOError:
        pass

    try:
        with open(LON_FILE) as f:
            lon = json.load(f)
    except IOError:
        pass

    try:
        with open(PATH_FILE) as f:
            path = json.load(f)
    except IOError:
        pass

    try:
        with open(CAMERA_FILE) as f:
            camera = json.load(f)
    except IOError:
        pass

    return {
        'group': group,
        'ctrl': ctrl,
        'lat': lat,
        'lon': lon,
        'path': path,
        'camera': camera,
    }

def ntune_lat_torque_get():
    try:
        with open(CONF_LAT_TORQUE_FILE) as f:
            return json.load(f)
    except IOError:
        return {}

def ntune_lat_indi_get():
    try:
        with open(CONF_LAT_INDI_FILE) as f:
            return json.load(f)
    except IOError:
        return {}

def ntune_scc_get():
    try:
        with open(CONF_SCC_FILE) as f:
            return json.load(f)
    except IOError:
        return {}

def ntune_common_params_get():
    try:
        with open(CONF_COMMON_FILE) as f:
            return json.load(f)
    except IOError:
        return {}
