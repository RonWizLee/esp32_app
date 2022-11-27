import 'package:esp32_app/device_page.dart';
import 'package:esp32_app/widgets/next_screen.dart';
import 'package:esp32_app/widgets/widgets.dart';
import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:page_transition/page_transition.dart';

class ConnectToDevice extends StatefulWidget {
  const ConnectToDevice({Key? key}) : super(key: key);

  @override
  State<ConnectToDevice> createState() => _ConnectToDeviceState();
}

class _ConnectToDeviceState extends State<ConnectToDevice> {

  @override
  void initState() {
    super.initState();
    FlutterBluePlus.instance.startScan(timeout: const Duration(seconds: 2));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Theme.of(context).backgroundColor,
      body: SafeArea(
        child: Padding(
          padding: const EdgeInsets.all(24.0),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Padding(
                padding: const EdgeInsets.symmetric(
                  horizontal: 15.0,
                  vertical: 20.0,
                ),
                child: Text(
                  '¡Bienvenido!',
                  style: Theme.of(context).textTheme.titleLarge,
                ),
              ),
              target(context, 'Conecte su dispositivo', listDevices(), height: 280.0),
            ],
          ),
        ),
      ),
    );
  }

  Widget listDevices() {
    return RefreshIndicator(
      color: Theme.of(context).focusColor,
      displacement: 20.0,
      onRefresh: () => FlutterBluePlus.instance.startScan(
        timeout: const Duration(seconds: 1),
      ),
      child: ListView(
        physics: const BouncingScrollPhysics(
            parent: AlwaysScrollableScrollPhysics()),
        children: [
          StreamBuilder<List<ScanResult>>(
            stream: FlutterBluePlus.instance.scanResults,
            initialData: const [],
            builder: (c, snapshot) => Column(
              children: snapshot.data!.map((r) {
                return ListTile(
                  contentPadding:
                      const EdgeInsets.symmetric(horizontal: 25.0),
                  title: Text(r.device.name),
                  subtitle: Text(r.device.id.toString()),
                  trailing: ElevatedButton(
                    child: const Text('Conectar'),
                    onPressed: () {
                      r.device.connect();
                      nextScreenReplace(
                        context,
                        DeviceScreen(device: r.device, isConnected: false),
                        PageTransitionType.rightToLeft,
                      );
                    },
                  ),
                );
              }).toList(),
            ),
          )
        ],
      ),
    );
  }

  // Widget _devicesConnected() {
  //   return StreamBuilder<List<BluetoothDevice>>(
  //     stream: Stream.periodic(const Duration(seconds: 1))
  //         .asyncMap((_) => FlutterBluePlus.instance.connectedDevices),
  //     initialData: const [],
  //     builder: (c, snapshot) => Column(
  //       children: snapshot.data!
  //           .map((d) => StreamBuilder<BluetoothDeviceState>(
  //                 stream: d.state,
  //                 initialData: BluetoothDeviceState.disconnected,
  //                 builder: (c, snapshot) {
  //                   if (snapshot.data == BluetoothDeviceState.connected) {
  //                     return ListTile(
  //                       title: Text(d.name),
  //                       subtitle: const Text('Conectado'),
  //                       trailing: ElevatedButton(
  //                         child: const Text('ABRIR'),
  //                         onPressed: () => nextScreenReplace(
  //                           context,
  //                           DeviceScreen(device: d, isConnected: true),
  //                           PageTransitionType.rightToLeft,
  //                         ),
  //                       ),
  //                     );
  //                   }
  //                   return Container();
  //                 },
  //               ))
  //           .toList(),
  //     ),
  //   );
  // }
}
