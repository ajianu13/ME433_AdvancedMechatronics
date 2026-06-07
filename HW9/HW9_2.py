import csv

t = [] # column 0
data1 = [] # column 1
# data2 = [] # column 2

with open('sigA.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        if len(row) < 2: 
            continue  # ADDED - skip rows that don't have 2 columns

        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column

        # data2.append(0.0)  # ADDED - CSV has no 3rd column, so insert placeholder

for i in range(len(t)):  # ADDED - range(t) is invalid; must use len(t)
    # print the data to verify it was read
    print(str(t[i]) + ", " + str(data1[i]))# + ", " + str(data2[i]))

plt.plot(t, data1, 'b-*')
plt.xlabel('Time [s]')
plt.ylabel('Signal')
plt.title('Signal vs Time')
plt.show()