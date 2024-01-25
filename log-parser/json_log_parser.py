import re
import json
import numpy
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from sklearn.cluster import KMeans
from sklearn.preprocessing import StandardScaler

#FE:ED:1A:71:09:7F is MOOK

json_data_file = 'FE%3AED%3A10%3A69%3A73%3A70-OEDCS'
json_data_file_list =[
#'Y:/Public Invention/data/FE%3AED%3A14%3A71%3A02%3A77-Stage2-save',
#'Y:/Public Invention/data/FE%3AED%3A01%3A7C%3A02%3A96-OEDCS',
#'Y:/Public Invention/data/#FE%3AED%3A01%3A7C%3A02%3A96-OEDCS#',
#'Y:/Public Invention/data/FE%3AED%3A1B%3A70%3A08%3A77-Stage2',
#'Y:/Public Invention/data/FE%3AED%3A1A%3A71%3A09%3A7F-OEDCS', #MOOK
#'Y:/Public Invention/data/FE%3AED%3A14%3A71%3A02%3A77-OEDCS',
#'Y:/Public Invention/data/FE%3AED%3A10%3A69%3A73%3A70-OEDCS',
#'Y:/Public Invention/data/FE%3AED%3A16%3A6B%3A72%3A67-OEDCS',
#'Y:/Public Invention/data/FE%3AED%3A1B%3A70%3A08%3A77-OEDCS',
#'Y:/Public Invention/data/FE%3AED%3A1B%3A70%3A0A%3A77-OEDCS'
#'Y:/Public Invention/data/FE%3AED%3A14%3A71%3A02%3A77-Stage2'
'./sample_data/critical_fault_sample_FE%3AED%3A1A%3A71%3A09%3A7F-OEDCS'
]
#'FE%3AED%3A14%3A71%3A02%3A77-OEDCS'
#'FE%3AED%3A10%3A69%3A73%3A70-OEDCS'
##
##with open(json_data_file, 'r') as file:
##    jstring = file.read()
###    print(jstring)
##json_re_pattern = re.compile(r'{\s*[^}]*\s*}',re.MULTILINE)
##
##remove_comma_re_pattern = re.replace(r'/\,(?=\s*?[\}\]])/g', '')
##matches = json_re_pattern.findall(jstring)
##for each in matches
##    remove_comma_re_pattern.
##
###print(matches)
##
##for match in matches:
##    try:
##        data = json.loads(match)
##        # Now 'data' is a dictionary representing a JSON object
##        print(data)
##    except json.JSONDecodeError as e:
##        print(f"Error decoding JSON: {e}")
##

print(f"pulling from {json_data_file_list}")

temperatures = []
temperatures_Getter_Stack = []
voltages = []
currents = []
resistances = []
ohms = []
times = []
dutycycles = []
target_c_values = []
setpoint_c_values = []
ramp_c_values = []
heater_c_values = []
transition_times = []
state_transitions = []

setpoint_vs_temperature_diffs = []
jstring = ""
malformed_entries_count = 0
processing_JSON_entry_count = 0
malformed_entries_count2 = 0
processing_JSON_entry_count2 = 0
decoding_JSON_entry_count = 0
decoding_JSON_entry_count2 = 0

for files in json_data_file_list:
    with open(files, 'r') as file:
        jstring += file.read()

# Define a regular expression pattern to match JSON objects
json_re_pattern = re.compile(r'{\s*[^}]*\s*}', re.MULTILINE)

# Define a regular expression pattern to remove trailing commas
remove_comma_re_pattern = re.compile(r',(?=\s*[\}\]])')

# Find all matches of the JSON pattern in the content
matches = json_re_pattern.findall(jstring)

# Remove trailing commas from each match
for i, match in enumerate(matches):
    matches[i] = remove_comma_re_pattern.sub('', match)

# Parse each modified JSON object
for match in matches:
    try:
       # Now 'data' is a dictionary representing a JSON object
        data = json.loads(match)
       # print(data)

       # Access values directly from the data dictionary
        temperature_value = data.get("StackC")
        voltage_value = data.get("StackV")
        current_value = data.get("StackA")
        time_value = data.get("TimeStamp")
        ohm_value = data.get("StackOhms")
        getter_value = data.get("GetterC")
        dutycycle = data.get("HeaterDutyCycle")
        target_c = data.get("TargetC")
        setpoint_c = data.get("SetpointC")
        ramp_c = data.get("RampC")
        heater_c = data.get("HeaterC")



        # Check if the required keys exist in the dictionary
        if temperature_value is not None and voltage_value is not None and current_value is not None and time_value is not None and getter_value is not None and ohm_value is not None:
            if ohm_value >= 0.001:
                temperatures.append(temperature_value)
                voltages.append(voltage_value)
                currents.append(current_value)
                times.append(time_value)
                ohms.append(ohm_value)
                temperatures_Getter_Stack.append(abs(temperature_value+getter_value)/2)
                dutycycles.append(dutycycle)
                target_c_values.append(target_c)
                setpoint_c_values.append(setpoint_c)
                ramp_c_values.append(ramp_c)
                heater_c_values.append(heater_c)
        else:
            malformed_entries_count += 1
            if malformed_entries_count < 10:
                print(f"Malformed dictionary entry: {data}")


    except json.JSONDecodeError as e:
        decoding_JSON_entry_count +=1
        if decoding_JSON_entry_count < 10:
            print(f"Error decoding JSON: {e}")
    except Exception as e:
        processing_JSON_entry_count +=1
        if processing_JSON_entry_count < 10:
            print(f"Error processing JSON entry: {e}")

for i,v in zip( currents,voltages):
    if i ==0:
        i = 0.1
    resistances.append( v/i)

for i in range(1, len(matches)):
    try:
        # Parse current and next JSON objects
        current_data = json.loads(matches[i - 1])
        next_data = json.loads(matches[i])

        # Capture state transitions
        state_transition = {
            "Time": current_data["RawMillis"],
            "Temperature": current_data["StackC"],
            "Voltage": current_data["StackV"],
            "Current": current_data["StackA"],
             "TimeStamp": current_data["TimeStamp"],
            "Next_Temperature": next_data["StackC"],
            "Next_Voltage": next_data["StackV"],
            "Next_Current": next_data["StackA"],
            "Next_TimeStamp": next_data["TimeStamp"],
            "Next_Time": next_data["RawMillis"],
        }

        # Append the state transition to the list
        state_transitions.append(state_transition)

    except json.JSONDecodeError as e:
        decoding_JSON_entry_count2 +=1
        if decoding_JSON_entry_count2 < 10:
            print(f"Error decoding JSON: {e}")
    except Exception as e:
        processing_JSON_entry_count2 +=1
        if processing_JSON_entry_count2 < 10:
            print(f"Error processing JSON entry: {e}")

print(f"Error decoding JSON count: {decoding_JSON_entry_count}")
print(f"Error decoding JSON count2: {decoding_JSON_entry_count2}")

print(f"Error processing JSON entry count: {processing_JSON_entry_count}")
print(f"Error processing JSON entry count2: {processing_JSON_entry_count2}")


# Calculate standard deviation for temperature, voltage, and current differences
temperature_diffs = [transition["Next_Temperature"] - transition["Temperature"] for transition in state_transitions]
voltage_diffs = [transition["Next_Voltage"] - transition["Voltage"] for transition in state_transitions]
current_diffs = [transition["Next_Current"] - transition["Current"] for transition in state_transitions]
time_diffs = [transition["Next_TimeStamp"] - transition["TimeStamp"] for transition in state_transitions]

# Calculate resistance differences
resistance_diffs = []
average_temperatures = []


for transition in state_transitions:
    try:
        if (transition["Next_Current"] == 0) and (transition["Current"] == 0):
            resistance_diffs.append((transition["Next_Voltage"] / 0.0001) - (transition["Voltage"] /  0.0001))
            #resistance_diffs.append(np.nan)  # Represent division by zero as NaN
        elif transition["Next_Current"] == 0:
            resistance_diffs.append((transition["Next_Voltage"] / 0.0001) - (transition["Voltage"] / transition["Current"]))
            #resistance_diffs.append(np.nan)  # Represent division by zero as NaN
        elif transition["Current"] == 0:
            resistance_diffs.append((transition["Next_Voltage"] / transition["Next_Current"]) - (transition["Voltage"] / 0.0001))
            #resistance_diffs.append(np.nan)  # Represent division by zero as NaN
        else:
            resistance_diffs.append((transition["Next_Voltage"] / transition["Next_Current"]) - (transition["Voltage"] / transition["Current"]))
        # Calculate average temperature
        average_temperature = (transition["Temperature"] + transition["Next_Temperature"]) / 2
        average_temperatures.append(average_temperature)

    except ZeroDivisionError:
        resistance_diffs.append(np.nan)  # Represent division by zero as NaN
kmeans = KMeans(n_clusters=3, random_state=42, n_init=10)


##
##
### Define the function to fit (you may need to change this depending on the shape of your data)
##def nonlinear_model(x, a, b, c):
##    return a * np.exp(-b * np.array(x)) + c
##try:
##    # Use curve_fit to fit the function to the data
##    popt, pcov = curve_fit(nonlinear_model, temperatures_Getter_Stack, ohms)
##except OptimizeWarning as e:
##    print(f"Optimization Warning: {e}")
##    popt = [1, 1, 1]  # Provide default values or handle it accordingly
##    pcov = None
### Print the optimized parameters
##print("Optimized Parameters:", popt)
### Generate y values using the fitted function
##fit_y = nonlinear_model(temperatures_Getter_Stack, *popt)

for i in range( 1, len(heater_c_values)):
    setpoint_vs_temperature_diffs.append( abs(heater_c_values[i]-setpoint_c_values[i]))

#count how many times the temperature was the same
steady_temps = 0
# Iterate through consecutive temperature values
for i in range(1, len(heater_c_values)):
    current_temperature = heater_c_values[i - 1]
    next_temperature = heater_c_values[i]

    # Check if a transition occurred
    if current_temperature != next_temperature:
        transition_time = times[i] - times[i - 1]

        if transition_time > 10:
            print( times[i])
            print( times[i - 1])
        else:
            transition_times.append(transition_time )
    else:
        steady_temps=steady_temps+1


print(f"times when temp was steady: {steady_temps} ")
# Calculate the average transition time
if transition_times:
    average_time = sum(transition_times) / len(transition_times)
    print(f"Average Transition Time: {average_time} seconds")  # unix time is in seconds
transition_times_median = np.median(transition_times)
setpoint_vs_temperature_diffs_median = np.median(setpoint_vs_temperature_diffs)
setpoint_vs_temperature_diffs_average = np.average(setpoint_vs_temperature_diffs)

transition_times_std = np.std(transition_times)
temperature_std = np.std(temperature_diffs)
setpoint_vs_temperature_diffs_std = np.std(setpoint_vs_temperature_diffs)
voltage_std = np.std(voltage_diffs)
current_std = np.std(current_diffs)
resistance_std = np.nanstd(resistance_diffs)

print(f"Average setpoint vs Temperature Differences: {setpoint_vs_temperature_diffs_average} degC")  # unix time is in seconds

print(f"Median of Temperature Transition time: {transition_times_median}")
print(f"Median of setpoint vs Temperature Differences: {setpoint_vs_temperature_diffs_median}")

# Print the results
print(f"Standard Deviation of Temperature transition time Differences: {transition_times_std}")
print(f"Standard Deviation of Temperature Differences: {temperature_std}")
print(f"Standard Deviation of setpoint vs Temperature Differences: {setpoint_vs_temperature_diffs_std}")
print(f"Standard Deviation of Voltage Differences: {voltage_std}")
print(f"Standard Deviation of Current Differences: {current_std}")
print(f"Standard Deviation of Resistance Differences: {resistance_std}")
print(f"Standard Deviation of TimeStamp Differences: {transition_times_std}")
# Print resistance vs average temperature
#for avg_temp, resistance in zip(average_temperatures, resistance_diffs):
#    print(f"Average Temperature: {avg_temp}, Resistance: {resistance}")

# Prepare the data for clustering
data = np.array(list(zip(average_temperatures, resistance_diffs)))
#data = data[~np.isnan(data).any(axis=1)]  # Remove rows with NaN values

# Standardize the data
scaler = StandardScaler()
data_scaled = scaler.fit_transform(data)

# Perform K-Means clustering
kmeans = KMeans(n_clusters=3, random_state=42)
kmeans.fit(data_scaled)

# Get cluster labels and centers
labels = kmeans.labels_
centers = kmeans.cluster_centers_

# Plotting subplots
fig, axs = plt.subplots(3, 2, figsize=(15, 12))

# Temperature vs Time
axs[0, 0].plot(range(len(heater_c_values)), heater_c_values, label='heater_c_values', color='red')
axs[0, 0].plot(range(len(setpoint_c_values)), setpoint_c_values, label='Setpoint', color='blue')
axs[0, 0].plot(range(len(transition_times)),transition_times,label='transition_times', color='orange')
axs[0, 0].set_title('heater_c_values vs Time')
axs[0, 0].set_xlabel('Time unitless')
axs[0, 0].set_ylabel('Temperature deg C')
axs[0, 0].legend()

# Voltage and Current vs Time
axs[0, 1].plot(range(len(voltages)), voltages, label='Voltage', color='blue')
axs[0, 1].plot(range(len(currents)), currents, label='Current', color='green')
axs[0, 1].set_title('Voltage and Current vs Time')
axs[0, 1].set_xlabel('Time')
axs[0, 1].set_ylabel('Value')
axs[0, 1].legend()

# Resistance vs Time
axs[1, 0].plot(times, resistances, label='Resistance', color='purple')
axs[1, 0].set_title('Resistance vs Time')
axs[1, 0].set_xlabel('Time')
axs[1, 0].set_ylabel('Resistance')
axs[1, 0].legend()
# Plot the original data and the fitted curve



# Change in Resistance vs Change in Temperature
##axs[1, 1].scatter(temperature_diffs, resistance_diffs, c='orange', label='Resistance Change', marker='o')
##axs[1, 1].set_title('Change in Resistance vs Change in Temperature')
##axs[1, 1].set_xlabel('Change in Temperature')
##axs[1, 1].set_ylabel('Change in Resistance')
##axs[1, 1].legend()
axs[1, 1].plot(range(len(time_diffs)),time_diffs,  c='orange', label='rate of data logging', marker='o')
axs[1, 1].set_title('rate of event messages over time')
axs[1, 1].set_xlabel('timestamp')
axs[1, 1].set_ylabel('rate of events')
axs[1, 1].legend()

### Cluster graph
##for i in range(3):  # Assuming 3 clusters
##    cluster_points = data[labels == i]
##    axs[2, 0].scatter(cluster_points[:, 0], cluster_points[:, 1], label=f'Cluster {i + 1}')
##
### Plot cluster centers
##axs[2, 0].scatter(centers[:, 0], centers[:, 1], marker='X', s=200, c='red', label='Cluster Centers')

# Label the axes
axs[2, 0].plot(temperatures_Getter_Stack, fit_y, 'r-', label='Fitted Curve')
axs[2, 0].scatter(temperatures_Getter_Stack, ohms, c='orange', label='Resistance Change', s=0.01, marker='o')
axs[2, 0].set_title('Resistance vs Temperature(Getter_Stack)')
axs[2, 0].set_xlabel('Average Temperature')
axs[2, 0].set_ylabel('Resistance')
axs[2, 0].legend()

# Label the axesresistance_diffs
axs[2, 1].scatter(average_temperatures, resistance_diffs, color='orange', s=0.01, label='Resistance Change')
axs[2, 1].set_xlabel('Average Temperature')
axs[2, 1].set_ylabel('Resistance difference')
axs[2, 1].legend()
# Adjust layout
plt.tight_layout()

# Show the plot
plt.show()
