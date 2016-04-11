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
    
    var time = ["1", "2", "3", "4"]
    var uvIndices : [Double] = [0.0, 0.0, 0.0, 0.0]

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
        
        
        let lineChartDataSet = LineChartDataSet(yVals: dataEntries, label: "Units Sold")
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
        
        //string coming in looks like 'reading:10:index:20'
        /*
        if(theString.containsString("reading:")){
            let stringParts = theString.componentsSeparatedByString(":")
            sonicView.uvReading.text = stringParts[1];
        }
        */
        if(theString.containsString("index:")){
            let stringParts = theString.componentsSeparatedByString(":")
            sonicView.uvIndex.text = stringParts[3];
            
            uvIndices.removeFirst();
            uvIndices.append(Double(stringParts[3])!);
            
            setChart(time, values: uvIndices)
            
        }
        
        
        
        
        print(theString)

    }


    // MARK: Helper

    func updatesonicView() {


    }

    func updateBean() {
        connectedBean?.setLedColor(sonicView.containerView.backgroundColor)
    }

}