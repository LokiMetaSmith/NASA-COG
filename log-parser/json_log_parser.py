import re
import json
import numpy
import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans
from sklearn.preprocessing import StandardScaler

json_data_file = 'FE%3AED%3A10%3A69%3A73%3A70-OEDCS'
json_data_file_list =[
#'Y:/Public Invention/data/FE%3AED%3A14%3A71%3A02%3A77-Stage2-save',
#'Y:/Public Invention/data/FE%3AED%3A01%3A7C%3A02%3A96-OEDCS',
#'Y:/Public Invention/data/#FE%3AED%3A01%3A7C%3A02%3A96-OEDCS#',
#'Y:/Public Invention/data/FE%3AED%3A1B%3A70%3A08%3A77-Stage2',
#'Y:/Public Invention/data/FE%3AED%3A1A%3A71%3A09%3A7F-OEDCS',
#'Y:/Public Invention/data/FE%3AED%3A14%3A71%3A02%3A77-OEDCS',
#'Y:/Public Invention/data/FE%3AED%3A10%3A69%3A73%3A70-OEDCS',
#'Y:/Public Invention/data/FE%3AED%3A16%3A6B%3A72%3A67-OEDCS',
#'Y:/Public Invention/data/FE%3AED%3A1B%3A70%3A08%3A77-OEDCS',
'Y:/Public Invention/data/FE%3AED%3A1B%3A70%3A0A%3A77-OEDCS'
#'Y:/Public Invention/data/FE%3AED%3A14%3A71%3A02%3A77-Stage2'
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
temperatures = []
voltages = []
currents = []
resistances = []
times = []
state_transitions = []
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
        # Check if the required keys exist in the dictionary
        if temperature_value is not None and voltage_value is not None and current_value is not None and time_value is not None:
            temperatures.append(temperature_value)
            voltages.append(voltage_value)
            currents.append(current_value)
            times.append(time_value)
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
            "Temperature": current_data["StackC"],
            "Voltage": current_data["StackV"],
            "Current": current_data["StackA"],
            "Next_Temperature": next_data["StackC"],
            "Next_Voltage": next_data["StackV"],
            "Next_Current": next_data["StackA"],
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


# Calculate resistance differences
resistance_diffs = []
average_temperatures = []


for transition in state_transitions:
    try:
        if (transition["Next_Current"] == 0) and (transition["Current"] == 0):
            #resistance_diffs.append((transition["Next_Voltage"] / 0.0001) - (transition["Voltage"] /  0.0001))
            resistance_diffs.append(np.nan)  # Represent division by zero as NaN
        elif transition["Next_Current"] == 0:
            #resistance_diffs.append((transition["Next_Voltage"] / 0.0001) - (transition["Voltage"] / transition["Current"]))
            resistance_diffs.append(np.nan)  # Represent division by zero as NaN
        elif transition["Current"] == 0:
            #resistance_diffs.append((transition["Next_Voltage"] / transition["Next_Current"]) - (transition["Voltage"] / 0.0001))
            resistance_diffs.append(np.nan)  # Represent division by zero as NaN
        else:
            resistance_diffs.append((transition["Next_Voltage"] / transition["Next_Current"]) - (transition["Voltage"] / transition["Current"]))
        # Calculate average temperature
        average_temperature = (transition["Temperature"] + transition["Next_Temperature"]) / 2
        average_temperatures.append(average_temperature)
            
    except ZeroDivisionError:
        resistance_diffs.append(np.nan)  # Represent division by zero as NaN


temperature_std = np.std(temperature_diffs)
voltage_std = np.std(voltage_diffs)
current_std = np.std(current_diffs)
resistance_std = np.std(resistance_diffs)


# Print the results
print(f"Standard Deviation of Temperature Differences: {temperature_std}")
print(f"Standard Deviation of Voltage Differences: {voltage_std}")
print(f"Standard Deviation of Current Differences: {current_std}")
print(f"Standard Deviation of Resistance Differences: {resistance_std}")

# Print resistance vs average temperature
#for avg_temp, resistance in zip(average_temperatures, resistance_diffs):
#    print(f"Average Temperature: {avg_temp}, Resistance: {resistance}")

# Prepare the data for clustering
data = np.array(list(zip(average_temperatures, resistance_diffs)))
data = data[~np.isnan(data).any(axis=1)]  # Remove rows with NaN values

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
axs[0, 0].plot(range(len(temperatures)), temperatures, label='Temperature', color='red')
axs[0, 0].set_title('Temperature vs Time')
axs[0, 0].set_xlabel('Time')
axs[0, 0].set_ylabel('Temperature')
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

# Change in Resistance vs Change in Temperature
axs[1, 1].scatter(temperature_diffs, resistance_diffs, c='orange', label='Resistance Change', marker='o')
axs[1, 1].set_title('Change in Resistance vs Change in Temperature')
axs[1, 1].set_xlabel('Change in Temperature')
axs[1, 1].set_ylabel('Change in Resistance')
axs[1, 1].legend()

# Cluster graph
for i in range(3):  # Assuming 3 clusters
    cluster_points = data[labels == i]
    axs[2, 0].scatter(cluster_points[:, 0], cluster_points[:, 1], label=f'Cluster {i + 1}')

# Plot cluster centers
axs[2, 0].scatter(centers[:, 0], centers[:, 1], marker='X', s=200, c='red', label='Cluster Centers')

# Label the axes
axs[2, 0].set_xlabel('Average Temperature')
axs[2, 0].set_ylabel('Resistance')
axs[2, 0].legend()

# Label the axesresistance_diffs
axs[2, 1].scatter(average_temperatures, resistance_diffs, color='orange', label='Resistance Change')
axs[2, 1].set_xlabel('Average Temperature')
axs[2, 1].set_ylabel('Resistance difference')
axs[2, 1].legend()
# Adjust layout
plt.tight_layout()

# Show the plot
plt.show()
