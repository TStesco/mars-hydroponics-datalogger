import processing.serial.*;

Serial myPort; //creates a software serial port on which you will listen to Arduino

int numReadings = 15; //keeps track of how many readings you'd like to take before writing the file. 
int readingCounter = 0; //counts each reading to compare to numReadings. 

Table dataTable;
String inSerial;
int lf = 10; // ASCII linefeed

void setup() {
  dataTable = new Table(); //create dataTable
  dataTable.addColumn("id", Table.INT);
  
  //the following adds columns for timestamps 
  dataTable.addColumn("year", Table.INT);
  dataTable.addColumn("month", Table.INT);
  dataTable.addColumn("day", Table.INT);
  dataTable.addColumn("hour", Table.INT);
  dataTable.addColumn("minute", Table.INT);
  dataTable.addColumn("second", Table.INT);
  
  //the following are columns for each data value.
  dataTable.addColumn("WaterTempC", Table.FLOAT);
  dataTable.addColumn("SoilMoisture", Table.FLOAT);
  dataTable.addColumn("AirTemp", Table.FLOAT);
  dataTable.addColumn("AirHum", Table.FLOAT);
  dataTable.addColumn("Dist1", Table.FLOAT);
  dataTable.addColumn("Dist2", Table.FLOAT);
  dataTable.addColumn("Dist3", Table.FLOAT);
  
  myPort = new Serial(this, Serial.list()[0], 9600); //set up your port to listen to the serial port
  myPort.bufferUntil(lf); //buffer
}
 
void serialEvent(Serial p) {
  String inSerial = p.readStringUntil(lf); //newline separator separates each Arduino loop.
  if (inSerial!= null) { //check if reading is valid
    inSerial = trim(inSerial); //gets rid of any whitespace or Unicode nonbreakable space 
    float sensorVals[] = float(split(inSerial, ',')); //parse based on ',' separtion
    
    TableRow newRow = dataTable.addRow(); //add a row for this new reading
    
    newRow.setInt("id", readingCounter+1);//record a unique identifier (the row's index)
    
    //record time stamp
    newRow.setInt("year", year());
    newRow.setInt("month", month());
    newRow.setInt("day", day());
    newRow.setInt("hour", hour());
    newRow.setInt("minute", minute());
    newRow.setInt("second", second());
    
    //record sensor information 
    newRow.setFloat("WaterTempC", sensorVals[0]);
    newRow.setFloat("SoilMoisture", sensorVals[1]);
    newRow.setFloat("AirTemp", sensorVals[2]);
    newRow.setFloat("AirHum", sensorVals[3]);
    newRow.setFloat("Dist1", sensorVals[4]);
    newRow.setFloat("Dist2", sensorVals[5]);
    newRow.setFloat("Dist3", sensorVals[6]);
    
    readingCounter++;
    
    //saves the table as a csv in the same folder as the sketch every numReadings. 
    if (readingCounter % numReadings ==0) //checks if it is time to write readings to file
    {
      String fileName = "/home/tom-s/sketchbook/Trial_" + str(readingCounter) + ".csv";
      saveTable(dataTable, fileName); //save .csv file
      dataTable.clearRows();
    }
   }
}

void draw() { 
  background(0); 
  text(Serial.list()[0], 10,50); 
} 
