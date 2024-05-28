import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/services.dart';

class SceneMonitor extends StatefulWidget {
  @override
  _SceneMonitorState createState() => _SceneMonitorState();
}

class _SceneMonitorState extends State<SceneMonitor> {
  @override
  final dbRef = FirebaseDatabase.instance.ref();

  Widget build(BuildContext context) {
    SystemChrome.setPreferredOrientations([
      DeviceOrientation.portraitUp,
      DeviceOrientation.portraitDown,
    ]);

    return Scaffold(
      appBar: AppBar(
        title: Text(
          'ESP8266 Monitor',
        ),
        backgroundColor: Colors.green,
      ),
      body: StreamBuilder(
          stream: dbRef
              .child("UsersData")
              .child("gLcuNbbCcqXPxqSXgJjhxmIT3sk1")
              .onValue,
          builder: (context, AsyncSnapshot snapshot) {
            if (snapshot.hasData &&
                !snapshot.hasError &&
                snapshot.data.snapshot.value != null) {
              return Column(
                children: [
                  SizedBox(
                    height: 20,
                  ),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      Column(
                        children: [
                          Padding(
                            padding: const EdgeInsets.all(8.0),
                            child: Text(
                              "Temperature",
                              style: TextStyle(
                                  fontSize: 20, fontWeight: FontWeight.bold),
                            ),
                          ),
                          Padding(
                            padding: const EdgeInsets.all(8.0),
                            child: Text(
                              snapshot.data.snapshot.value['temperature']
                                      .toString() +
                                  "°C",
                              // dummy value
                              style: TextStyle(
                                fontSize: 20,
                                fontWeight: FontWeight.bold,
                              ),
                            ),
                          ),
                        ],
                      )
                    ],
                  ),
                  SizedBox(
                    height: 20,
                  ),
                  Column(
                    children: [
                      Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Text(
                          "Humidity",
                          style: TextStyle(
                              fontSize: 20, fontWeight: FontWeight.bold),
                        ),
                      ),
                      Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Text(
                          snapshot.data.snapshot.value['humidity'].toString() +
                              "%", // dummy value
                          style: TextStyle(
                              fontSize: 20, fontWeight: FontWeight.bold),
                        ),
                      ),
                    ],
                  ),
                  SizedBox(
                    height: 20,
                  ),
                  Column(
                    children: [
                      Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Text(
                          "Heat Index",
                          style: TextStyle(
                              fontSize: 20, fontWeight: FontWeight.bold),
                        ),
                      ),
                      Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Text(
                          snapshot.data.snapshot.value['heatindex'].toString() +
                              "°C",
                          // dummy value
                          style: TextStyle(
                            fontSize: 20,
                            fontWeight: FontWeight.bold,
                          ),
                        ),
                      ),
                    ],
                  ),
                  SizedBox(
                    height: 20,
                  ),
                  Column(
                    children: [
                      Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Text(
                          "Gas Level",
                          style: TextStyle(
                              fontSize: 20, fontWeight: FontWeight.bold),
                        ),
                      ),
                      Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Text(
                          snapshot.data.snapshot.value['gasvalue'].toString(),

                          // dummy value
                          style: TextStyle(
                            fontSize: 20,
                            fontWeight: FontWeight.bold,
                          ),
                        ),
                      ),
                    ],
                  ),
                  SizedBox(
                    height: 20,
                  ),
                  Column(
                    children: [
                      Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Text(
                          "Gas Notification",
                          style: TextStyle(
                              fontSize: 20, fontWeight: FontWeight.bold),
                        ),
                      ),
                      if (snapshot.data.snapshot.value['gasvalue'] <= 45) ...[
                        Text("No Gas Leakage"),
                      ] else if (snapshot.data.snapshot.value['gasvalue'] >
                          45) ...[
                        Text("Warning! Gas leak detected")
                      ],
                    ],
                  ),
                  SizedBox(
                    height: 20,
                  ),
                  Column(
                    children: [
                      Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Text(
                          "Fire Notification",
                          style: TextStyle(
                              fontSize: 20, fontWeight: FontWeight.bold),
                        ),
                      ),
                      if (snapshot.data.snapshot.value['flamevalue'] ==
                          true) ...[
                        Text("No Fire Detected"),
                      ] else if (snapshot.data.snapshot.value['flamevalue'] ==
                          false) ...[
                        Text("Warning! Fire was detected")
                      ],
                    ],
                  ),
                ],
              );
            } else {
              return Container();
            }
          }),
    );
  }
}
