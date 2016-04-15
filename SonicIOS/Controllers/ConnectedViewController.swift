//
//  ConnectedViewController.swift
//  Cool Beans
//
//  Created by Kyle on 11/14/14.
//  Copyright (c) 2014 Kyle Weiner. All rights reserved.
//

import UIKit
import Charts

class ConnectedViewController: UIViewController, PTDBeanDelegate {

    var connectedBean: PTDBean?
    
    var time : [String] = []
    var uvIndices : [Double] = []

    @IBOutlet weak var scrollView: UIScrollView!
    @IBOutlet weak var sonicView: SonicView!
    
   

    // MARK: Lifecycle

    override func viewDidLoad() {
        super.viewDidLoad()

        // Update the name label.
        sonicView.nameLabel.text = connectedBean?.name
        
        sonicView.myController = self;
        
    }

    
    func setChart(dataPoints: [String], values: [Double]) {
        
        var dataEntries: [ChartDataEntry] = []
        
        for i in 0..<dataPoints.count {
            let dataEntry = ChartDataEntry(value: values[i], xIndex: i)
            dataEntries.append(dataEntry)
        }
        
        
        let lineChartDataSet = LineChartDataSet(yVals: dataEntries, label: "UV Index")
        let lineChartData = LineChartData(xVals: dataPoints, dataSet: lineChartDataSet)
        
       
        
        sonicView.lineChartView.data = lineChartData
        
    }

    
    
    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        connectedBean?.readTemperature()
        setChart(time, values: uvIndices)
    }

    // MARK: Actions

    func updateAcc(){
        connectedBean?.readTemperature()
        connectedBean?.readAccelerationAxes()
    }

    // MARK: PTDBeanDelegate
    
    func bean(bean: PTDBean!, serialDataReceived data:NSData) {
        let theString:String = NSString(data: data, encoding: NSASCIIStringEncoding)! as String
        
        //string coming in looks like 'index:20'

        if(theString.containsString("index:")){
            let stringParts = theString.componentsSeparatedByString(":")
            sonicView.uvIndex.text = stringParts[1]
            
            if(uvIndices.count >= 10){
                time.removeFirst()
                uvIndices.removeFirst()
            }
            time.append(getCurrentTime())
            uvIndices.append(Double(stringParts[1])!)
            
            setChart(time, values: uvIndices)
            
        }
        
        
        if(theString.containsString("timer_at_zero")){
            
            let notification = UILocalNotification()
            notification.alertBody = "Put on sunscreen!" // text that will be displayed in the notification
            notification.alertAction = "open" // text that is displayed after "slide to..." on the lock screen - defaults to "slide to view" → I don’t think we’re actually using this
            notification.fireDate = NSDate() // date when notification should be fired (NSDate() defaults to current date and time)
            notification.soundName = UILocalNotificationDefaultSoundName // play default sound (don’t think this’ll work unless we add “.Sound” to the AppDelegate)
            notification.category = "TODO_CATEGORY" // i don’t think we’re using this
            UIApplication.sharedApplication().scheduleLocalNotification(notification) // schedules the notification
            
        }
        
        
        
        print(theString)

    }
    
    func getCurrentTime() -> String{
        let date = NSDate()
        let calendar = NSCalendar.currentCalendar()
        let hour = calendar.component(.Hour, fromDate: date)
        let minutes = calendar.component(.Minute, fromDate: date)
        
        let string = String(hour) + ":" + String(minutes)
        
        return string;
        
        
    }


    // MARK: Helper

    func updatesonicView() {


    }

    func updateBean() {
        connectedBean?.setLedColor(sonicView.containerView.backgroundColor)
    }

}