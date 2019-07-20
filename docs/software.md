#Software

## 1.1) Software on the ESP
Download the starter code from the following links:
1. [Code](https://drive.google.com/open?id=1ycZDQ5II_bTpF3VxJhD5JPJp1ixwjPtG) for ESP32 connected to camera.
2. [Code](https://drive.google.com/open?id=1py7Ks53zTSZ2QU7lbmBKup8znGt8wMLM) for ESP32 control center (lamp, pump, POST and GET from server).

Make sure to change the WiFi SSID and password to match the network you wish to connect to.

## 1.2) Software on the server (`608dev.net`)
The server on `608dev.net` plays a central role in our device: it is the central hub that receives updates from the client (the ESP32) about the status of the pump and lamp, as well as the sensor readings. The server also determines the current control state for the pump and lamp, and relays that information back to the ESP32. Finally, `608dev.net` serves as a convenient web host for the Web UI, which acts as the control center from a user's perspective, allowing them the change the settings, schedule lamp and pump times, and check on the status of their plant.
Since the server performs many functions, we will need to separate each function into its own file, with its own `request_handler`. To make this more manageable and easier to maintain in the future, it's recommended to use a folder hierarchy to organize and declare what each endpoint does (e.g. for endpoints that update values, place them in an `update/` folder, or name the endpoint `update.py`).
The following is the folder structure that we have used in this project, as well as brief descriptions with what each endpoint or folder is used for.

- `__HOME__/projectname`
- `camera/`
    - `read.py`
    - `update.py`
- `control/`
    - `override/`
    - `lamp.py`
    - `pump.py`
- `power/`
- `schedule/`
- `sensors/`
- `status/`
- `control.py`
- `utils.py`

To get this project structure, download the server code [here](https://drive.google.com/open?id=1EhXFokElaLBPYzhdOF5WYs9qGVTQNd9V) and place it in the appropriate directory.

What's important to note is that for each effector, i.e. each component that can affect the plant, it is necessary to have an endpoint where the ESP32 is able to update the current status of that effector, as well as an endpoint for the ESP32 to read what the intended state of that effector is. This is implemented above in the `status/` and `control/` folders, respectively.


Central are the endpoints that determine what the intended states for the effectors. These endpoints integrate the control logic, including the scheduling, manual overrides, and moisture bounds. This is one of the reasons that it is important to store all the information in databases, so that it may be accessed by multiple different endpoints to perform functions accordingly.


Lastly, for user changeable settings, it is important to have a `read.py` and an `update.py` (or analogs — a read current setting endpoint, and a change setting endpoint). The "read" endpoint is used by both the Web UI to display to the user what the current setting is (so that they may determine whether that setting is appropriate), as well as the ESP32 to determine what settings may need to be changed on the client. The update endpoint interacts primarily with the Web UI, so that the user can change the settings in an intuitive manner. This is dependent on the setting, but generally, having switches or input boxes with easily understandable instructions will work.