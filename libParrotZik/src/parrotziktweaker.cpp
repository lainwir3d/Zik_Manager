#include "parrotziktweaker.h"

//using namespace QtBluetooth;

ParrotZikTweeker::ParrotZikTweeker()
{
    localDevice = new QBluetoothLocalDevice();

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)), this, SLOT(deviceDiscovered(const QBluetoothDeviceInfo&)));
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(deviceDiscoveryFinished()));

    commonInit();
    connect(localDevice, SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)), this, SLOT(bluetoothModeChanged(QBluetoothLocalDevice::HostMode)));
    connect(this, SIGNAL(answerReceived()), &waitForAnswerLoop, SLOT(quit()));

    if(localDevice->isValid()) {
        if(localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff){
            qDebug() << "Bluetooth local device is off.";
            m_bluetoothOn = false;
        }else{
            qDebug() << "Bluetooth local device is on.";
            m_bluetoothOn = true;

            autoconnection_timer->setInterval(0);
            autoconnection_timer->start();
            //this->startAutoConnection();
        }
    }else{
#if defined(Q_OS_WINRT)
        qDebug() << "WinRT detected, let's just assume we have a working bluetooth on the device since we don't have any info on that.";
        m_bluetoothOn = true;
        autoconnection_timer->setInterval(0);
        autoconnection_timer->start();
#else
        qDebug() << "No valid local device";
        m_bluetoothOn = false;
#endif
    }

}

ParrotZikTweeker::ParrotZikTweeker(QString btAddr)
{
    localDevice = new QBluetoothLocalDevice();

    commonInit();
    connect(localDevice, SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)), this, SLOT(bluetoothModeChanged(QBluetoothLocalDevice::HostMode)));
    connect(this, SIGNAL(answerReceived()), &waitForAnswerLoop, SLOT(quit()));

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);


    if(localDevice->isValid()) {
        if(localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff){
            qDebug() << "Bluetooth local device is off.";
            m_bluetoothOn = false;
        }else{
            qDebug() << "Bluetooth local device is on.";
            m_bluetoothOn = true;

            QBluetoothAddress a(btAddr);

            QBluetoothDeviceInfo d(a, "Zik", 1);

            QBluetoothServiceInfo s;
            s.setDevice(d);

            //QBluetoothUuid u(QString("0ef0f502-f0ee-46c9-986c-54ed027807fb"));  // ZIK 1
            QBluetoothUuid u(QString("8b6814d3-6ce7-4498-9700-9312c1711f63"));  // ZIK 2
            s.setServiceUuid(u);
            startClient(s);
        }
    }else{
#if defined(Q_OS_WINRT)
        qDebug() << "WinRT detected, let's just assume we have a working bluetooth on the device since we don't have any info on that.";
        m_bluetoothOn = true;
        autoconnection_timer->setInterval(0);
        autoconnection_timer->start();

        QBluetoothAddress a(btAddr);
        QBluetoothDeviceInfo d(a, "Zik", 1);
        QBluetoothServiceInfo s;
        s.setDevice(d);
        //QBluetoothUuid u(QString("0ef0f502-f0ee-46c9-986c-54ed027807fb"));  // ZIK 1
        QBluetoothUuid u(QString("8b6814d3-6ce7-4498-9700-9312c1711f64"));  // ZIK 2
        s.setServiceUuid(u);
        m_model = "ZIK3";
        modelChanged();
        setDefaultCapabilities();
        startClient(s);
#else
        qDebug() << "No valid local device";
#endif
    }

}

void ParrotZikTweeker::closeBTConnectionsBeforeQuit()
{
    if(!socket) return;

	if (socket->state() == QBluetoothSocket::ConnectedState) {
		qDebug() << "Disconnecting from BT service before quitting";
		socket->disconnectFromService();
	}

	qDebug() << "We do not seem to be connected, but just in case... stopping socket before quitting.";
	socket->abort();
	socket->close();
}

// only for android
QList<DevStruct> ParrotZikTweeker::getKnownDevices() {
    QList<DevStruct> result;
    //QString fmt("%1 %2");
    QString fmt("%1");

#ifdef Q_OS_ANDROID
    // Query via Android Java API.
    QAndroidJniObject adapter=QAndroidJniObject::callStaticObjectMethod("android/bluetooth/BluetoothAdapter","getDefaultAdapter","()Landroid/bluetooth/BluetoothAdapter;"); // returns a BluetoothAdapter
    if (checkException("BluetoothAdapter.getDefaultAdapter()",&adapter)) {
        return result;
    }
    QAndroidJniObject pairedDevicesSet=adapter.callObjectMethod("getBondedDevices","()Ljava/util/Set;"); // returns a Set<BluetoothDevice>
    if (checkException("BluetoothAdapter.getBondedDevices()",&pairedDevicesSet)) {
        return result;
    }
    jint size=pairedDevicesSet.callMethod<jint>("size");
    checkException("Set<BluetoothDevice>.size()", &pairedDevicesSet);
    if (size>0) {
        QAndroidJniObject iterator=pairedDevicesSet.callObjectMethod("iterator","()Ljava/util/Iterator;"); // returns an Iterator<BluetoothDevice>
        if (checkException("Set<BluetoothDevice>.iterator()",&iterator)) {
            return result;
        }
        for (int i=0; i<size; i++) {
            QAndroidJniObject dev=iterator.callObjectMethod("next","()Ljava/lang/Object;"); // returns a BluetoothDevice
            if (checkException("Iterator<BluetoothDevice>.next()",&dev)) {
                continue;
            }
            QString address=dev.callObjectMethod("getAddress","()Ljava/lang/String;").toString(); // returns a String
            QString name=dev.callObjectMethod("getName","()Ljava/lang/String;").toString(); // returns a String
            //result.append(fmt.arg(address).arg(name));
            DevStruct device_s;
            device_s.address = address;
            device_s.name = name;
            device_s.uuids = QStringList();

            if(address.toLower().startsWith("90:03:b7") || address.toLower().startsWith("a0:14:3d") || address.toLower().startsWith("f0:18:e5:9f") || address.toLower().startsWith("88:82:b4")){
                qDebug() << "found bonded parrot device";
                QAndroidJniObject UuidsParcel = dev.callObjectMethod("getUuids", "()[Landroid/os/ParcelUuid;");
                if (checkException("BluetoothDevice.getUuids()",&UuidsParcel)) {
                    qDebug("shit");
                    return result;
                }

                const jobjectArray uuidsArray = UuidsParcel.object<jobjectArray>();
                QAndroidJniEnvironment qjniEnv;
                const int n = qjniEnv->GetArrayLength(uuidsArray);

                //qDebug() << "cool. " << n << " elements";

                for(int i=0; i< n;i++){
                    jobject localRef = qjniEnv->GetObjectArrayElement(uuidsArray, i);
                    QAndroidJniObject uuidpar_o = QAndroidJniObject::fromLocalRef(localRef);

                    QString uuidpar_s = uuidpar_o.callObjectMethod("toString", "()Ljava/lang/String;").toString();
                    //if (checkException("BluetoothDevice.getUuids()",&UuidsParcel)) {
                    //    qDebug("shit");
                    //    return result;
                    //}
                    qDebug() << "nice. uuid=" << uuidpar_s;
                    device_s.uuids.append(uuidpar_s);

                    /*
                    QAndroidJniObject uuid_o = uuidpar_o.callObjectMethod("getUuid", "()Ljava/util/UUID;");

                    qDebug() << "very nice.";

                    QString uuid_s = uuid_o.callObjectMethod("toString", "()Ljava/lang/String;").toString();

                    qDebug() << "Over and over. uuid=" << uuid_s;
                    */

                    qDebug() << "TODO: check for JNI errors.";

                }
            }
            result.append(device_s);

        }
    }

#endif

    return result;
}

#ifdef Q_OS_ANDROID
bool ParrotZikTweeker::checkException(const char* method, const QAndroidJniObject* obj) {
    static QAndroidJniEnvironment env;
    bool result=false;
    if (env->ExceptionCheck()) {
        qCritical("Exception in %s",method);
        env->ExceptionDescribe();
        env->ExceptionClear();
        result=true;
    }
    if (!(obj==NULL || obj->isValid())) {
        qCritical("Invalid object returned by %s",method);
        result=true;
    }
    return result;
}
#endif

void ParrotZikTweeker::bluetoothModeChanged(QBluetoothLocalDevice::HostMode mode)
{
    if(mode == QBluetoothLocalDevice::HostPoweredOff){
        if(false != m_bluetoothOn){
            qDebug("Bluetooth has been turned off.");
            m_bluetoothOn = false;
            emit bluetoothOnChanged();

            if(connection){
                qDebug("Bluetooth has been turned off. Stopping active connection");
                stopClient();
            }

            if(autoconnection_timer->isActive()){
                qDebug("Bluetooth has been turned off. Stopping active auto-connection");
                autoconnection_timer->stop();
            }
        }
    }else{
        if(true != m_bluetoothOn){
            qDebug("Bluetooth has been turned on.");
            m_bluetoothOn = true;
            emit bluetoothOnChanged();

            if(viewVisible()) this->startAutoConnection();
        }
    }
}

void ParrotZikTweeker::connectTo(QString btaddr)
{
    QBluetoothDeviceInfo dev;
    QBluetoothAddress a = QBluetoothAddress(btaddr);

    dev = QBluetoothDeviceInfo(a, "Zik", 1);

    if(dev.isValid()){
        if(!serviceDiscoveryAgent) {
            serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent();
            connect(serviceDiscoveryAgent, SIGNAL(serviceDiscovered(const QBluetoothServiceInfo&)), this, SLOT(serviceDiscovered(const QBluetoothServiceInfo&)));
            connect(serviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(serviceDiscoveryFinished()));
        }else if(serviceDiscoveryAgent->isActive()){
            qDebug("Service discovery in progress. Stopping it.");
            serviceDiscoveryAgent->stop();
        }else{
            qDebug("No service discovery in progress. Starting one.");
        }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)) || defined(QTBLUETOOTH_BACKPORT)
        serviceDiscoveryAgent->setRemoteAddress(a);
        qDebug() << "Parrot device found (" << serviceDiscoveryAgent->remoteAddress().toString() << "). Starting service discovery";
        serviceDiscoveryAgent->start(QBluetoothServiceDiscoveryAgent::MinimalDiscovery);
#else
        serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(dev.address());
        connect(serviceDiscoveryAgent, SIGNAL(serviceDiscovered(const QBluetoothServiceInfo&)), this, SLOT(serviceDiscovered(const QBluetoothServiceInfo&)));
        connect(serviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(serviceDiscoveryFinished()));
        qDebug() << "(QtBluetooth < 5.2.0): Parrot device found (" << a.toString() << "). Starting service discovery";
        serviceDiscoveryAgent->start();
#endif
    }else{
        qDebug() << "Device not valid";
    }
}

void ParrotZikTweeker::startAutoConnection()
{

    QBluetoothDeviceInfo dev;
    QBluetoothAddress a;

#if defined(Q_OS_ANDROID)
    qDebug("Android device detected. Connecting to first Parrot Zik device detected...");
    QList<DevStruct> btdevlist = getKnownDevices();
    qDebug("TODO: rename object correctly to reflect type");

    for(int i=0;i<btdevlist.size();i++){
        DevStruct d = btdevlist.at(i);

        QString address = d.address;
        QString name = d.name;
        QStringList uuids = d.uuids;
        if(address.toLower().startsWith("90:03:b7") || address.toLower().startsWith("a0:14:3d") || address.toLower().startsWith("f0:18:e5:9f") || address.toLower().startsWith("88:82:b4")){
            qDebug() << "Parrot bt device found on android: " << address << " " << name;

            // service discovery on android returns weird UUIDs
            /*
            if(!serviceDiscoveryAgent) {
                serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent();
                connect(serviceDiscoveryAgent, SIGNAL(serviceDiscovered(const QBluetoothServiceInfo&)), this, SLOT(serviceDiscovered(const QBluetoothServiceInfo&)));
                connect(serviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(serviceDiscoveryFinished()));
            }else{
                qDebug("Stopping current service discovery");
                serviceDiscoveryAgent->stop();
            }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)) || defined(QTBLUETOOTH_BACKPORT)
            serviceDiscoveryAgent->setRemoteAddress(QBluetoothAddress(address));
#else
            serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(device.address());
#endif

            serviceDiscoveryAgent->start(QBluetoothServiceDiscoveryAgent::MinimalDiscovery);
            return;
            */

            qDebug() << "Service discovery on android not working correctly (missing custom UUIDs). Resorting to audio service scanning + version string parsing for zik + model detection, might be unreliable.";

            bool handsfreeOk = false;
            bool phonebookOk = false;
            bool audiosinkOk = false;
            bool avcontrollerOk = false;
            bool avtargetOk = false;

            for(int j=0; j<uuids.length(); j++){
                QString u = uuids.at(j);

                if(u.startsWith("0000111e")){
                    qDebug("Found hands free service");
                    handsfreeOk = true;
                }else if(u.startsWith("0000112e")){
                    qDebug("Found phone book service");
                    phonebookOk = true;
                }else if(u.startsWith("0000110b")){
                    qDebug("Found audio sink service");
                    audiosinkOk = true;
                }else if(u.startsWith("0000110e")){
                    qDebug("Found av rc controller service");
                    avcontrollerOk = true;
                }else if(u.startsWith("0000110c")){
                    qDebug("Found av rc target service");
                    avtargetOk = true;
                }




            }

            if(handsfreeOk && audiosinkOk && avcontrollerOk){
                qDebug() << "This is probably a Zik device (handsfree + audiosink + A/V RC Controller), going ahead with version string parsing...";

                QString uuid_str = "";
                if(name.contains("V3.")){
                    qDebug() << "Zik 3 detected.";

                    uuid_str = "8b6814d3-6ce7-4498-9700-9312c1711f64";
                    m_model = "ZIK3";
                    modelChanged();
                }else if(name.contains("V2.")){
                    qDebug() << "Zik 2 detected.";

                    uuid_str = "8b6814d3-6ce7-4498-9700-9312c1711f63";
                    m_model = "ZIK2";
                    modelChanged();
                }else if(name.contains("V1.")){
                    qDebug() << "Zik 1 detected.";

                    uuid_str = "0ef0f502-f0ee-46c9-986c-54ed027807fb";
                    m_model = "ZIK1";
                    modelChanged();
                }else{
                    qDebug() << "Zik not detected, probably another Parrot device.";
                }

                if(!uuid_str.isEmpty()){
                    qDebug() << "Connecting to (probably) detected " << m_model << " device on Android";
                    QBluetoothAddress a(address);
                    QBluetoothDeviceInfo d(a, "Zik", 1);
                    QBluetoothServiceInfo s;
                    s.setDevice(d);
                    QBluetoothUuid u(uuid_str);  // ZIK 2
                    s.setServiceUuid(u);

                    setDefaultCapabilities();
                    startClient(s);
                    return;
                }else{
                    qDebug() << "Could not determine Zik version based on version string... giving up";
                }
            }else{
                qDebug() << "Necessary bt services not found on device. This is probably not a Zik device";
            }

            /*
            QString uuid_str = "8b6814d3-6ce7-4498-9700-9312c1711f64";

            //discoveryAgent->stop()

            QBluetoothAddress a(address);
            QBluetoothDeviceInfo d(a, "Zik", 1);
            QBluetoothServiceInfo s;
            s.setDevice(d);
            //QBluetoothUuid u(QString("0ef0f502-f0ee-46c9-986c-54ed027807fb"));  // ZIK 1
            QBluetoothUuid u(uuid_str);  // ZIK 2
            s.setServiceUuid(u);
            m_model = "ZIK3";
            modelChanged();
            setDefaultCapabilities();
            startClient(s);
*/
        }
    }

#elif defined(Q_OS_WINRT)
    if(demoMode()){
        qDebug("Demo mode started. Cancelling connection.");
    }else{
        qDebug() << "WinRT detected. No info on connected devices except through 'fake' device discovery. Doing it then.";
		if (!serviceDiscoveryAgent) {
			serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent();
			connect(serviceDiscoveryAgent, SIGNAL(serviceDiscovered(const QBluetoothServiceInfo&)), this, SLOT(serviceDiscovered(const QBluetoothServiceInfo&)));
			connect(serviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(serviceDiscoveryFinished()));
		}
        discoveryAgent->start();
    }
    return;
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)) || defined(QTBLUETOOTH_BACKPORT)

    QList<QBluetoothAddress> l = localDevice->connectedDevices();
    for(int i=0; i< l.length(); i++){
        if(l[i].toString().toLower().startsWith("90:03:b7") || l[i].toString().toLower().startsWith("a0:14:3d") || l[i].toString().toLower().startsWith("f0:18:e5:9f") || l[i].toString().toLower().startsWith("88:82:b4")){
            a = l[i];
            break;
        }
    }

    if(a.isNull()){
        qDebug() << "Connected device not found.";

        /*
        discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
        connect(discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)), this, SLOT(deviceDiscovered(const QBluetoothDeviceInfo&)));
        discoveryAgent->start();
        */
    }else{
        dev = QBluetoothDeviceInfo(a, "Zik", 1);
    }


#else


#ifdef Q_OS_LINUX

    qDebug("QtBluetooth < 5.3.0. Checking for connected bluetooth device using D-Bus.");

    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusInterface dbus_iface("org.bluez", "/",
                              "org.bluez.Manager", bus);
    if(!dbus_iface.isValid()){
        qDebug("Dbus interface invalid");
    }
    QDBusReply<QDBusObjectPath > defaultAdapter_reply = dbus_iface.call("DefaultAdapter");

    if (defaultAdapter_reply.isValid())
        {
            QDBusObjectPath defaultAdapter = defaultAdapter_reply.value();
            QString defaultAdapterPath = defaultAdapter.path();
            //qDebug() << defaultAdapterPath;

            QDBusInterface adapterDbusInterface("org.bluez",defaultAdapterPath,"org.bluez.Adapter",bus);
            QDBusReply<QList<QDBusObjectPath> > listdevices_reply = adapterDbusInterface.call("ListDevices");

            if(listdevices_reply.isValid())
            {
                QList<QDBusObjectPath> remoteDevs = listdevices_reply.value();

                for(int i=0; i<remoteDevs.length(); i++){
                    //qDebug() << remoteDevs[i].path();

                    QDBusInterface controlDbusInterface("org.bluez", remoteDevs[i].path(),"org.bluez.Control",bus);
                    QDBusReply<bool > devConnected_reply = controlDbusInterface.call("IsConnected");

                    //qDebug() << devConnected_reply.value();

                    if(devConnected_reply.value()){
                        QDBusInterface deviceDbusInterface("org.bluez", remoteDevs[i].path(),"org.bluez.Device",bus);
                        QDBusReply<QMap<QString, QVariant> > properties_reply = deviceDbusInterface.call("GetProperties");
                        QMap<QString, QVariant> properties = properties_reply.value();

                        QString dname = properties["Alias"].toString();
                        QString daddress = properties["Address"].toString();
                        qDebug() << dname << " (" << daddress << ") " <<"is connected";

                        if(daddress.toLower().startsWith("90:03:b7") || daddress.toLower().startsWith("a0:14:3d") || daddress.toLower().startsWith("f0:18:e5:9f") || daddress.toLower().startsWith("88:82:b4")){

                            a = QBluetoothAddress(daddress);
                            dev = QBluetoothDeviceInfo(a, "Zik", 1);

                            break;
                        }
                    }
                }
            }
            else
            {
                qDebug() << listdevices_reply.error();
            }
        }
        else
        {
            qDebug() << defaultAdapter_reply.error();
        }
#else
    qDebug("Platform not supported.");
    /*
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)), this, SLOT(deviceDiscovered(const QBluetoothDeviceInfo&)));
    discoveryAgent->start();
    */
#endif //Q_OS_LINUX

#endif // QT 5.3

    if(dev.isValid()){
        if(demoMode()){
            qDebug("Demo mode started. Cancelling connection.");
        }else{

            if(!serviceDiscoveryAgent) {
                serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent();
                connect(serviceDiscoveryAgent, SIGNAL(serviceDiscovered(const QBluetoothServiceInfo&)), this, SLOT(serviceDiscovered(const QBluetoothServiceInfo&)));
                connect(serviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(serviceDiscoveryFinished()));
            }else if(serviceDiscoveryAgent->isActive()){
                qDebug("Service discovery in progress. Stopping it.");
                serviceDiscoveryAgent->stop();
            }else{
                qDebug("No service discovery in progress. Starting one.");
            }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)) || defined(QTBLUETOOTH_BACKPORT)
            serviceDiscoveryAgent->setRemoteAddress(a);
            qDebug() << "Parrot device found (" << serviceDiscoveryAgent->remoteAddress().toString() << "). Starting service discovery";
            serviceDiscoveryAgent->start(QBluetoothServiceDiscoveryAgent::MinimalDiscovery);
#else
            serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(dev.address());
            connect(serviceDiscoveryAgent, SIGNAL(serviceDiscovered(const QBluetoothServiceInfo&)), this, SLOT(serviceDiscovered(const QBluetoothServiceInfo&)));
            connect(serviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(serviceDiscoveryFinished()));
            qDebug() << "(QtBluetooth < 5.2.0): Parrot device found (" << a.toString() << "). Starting service discovery";
            serviceDiscoveryAgent->start();
#endif


            /*
            QBluetoothServiceInfo service;
            service.setDevice(dev);
            QBluetoothUuid u(QString("0ef0f502-f0ee-46c9-986c-54ed027807fb"));  // ZIK 1
            QBluetoothUuid u(QString("8b6814d3-6ce7-4498-9700-9312c1711f63"));  // ZIK 2

            service.setServiceUuid(u);
            */
        }
    }else{
        if(demoMode()){
            qDebug("Demo mode is on. Stopping autoconnection.");
        }else if(bluetoothOn() && viewVisible()){
            autoconnection_timer->setInterval(3000);
            qDebug() << "Zik NOT found, retrying in " << autoconnection_timer->interval() << "ms.";
            autoconnection_timer->start();
        }
    }

}

void ParrotZikTweeker::commonInit()
{

    connection = 0;

    this->socket = 0;
    this->serviceDiscoveryAgent = 0;

    this->m_model = "invalid";

    this->m_autoPowerOff = 0;
    this->m_battery = "0";
    this->m_version = "0";
    this->m_artist = "";
    this->m_album = "";
    this->m_song = "";
    this->m_genre = "";

    this->m_demoMode = false;
    this->m_downloadInProgress = false;
    this->m_smartAudioTune = "false";

    this->m_surroundMode = false;

    this->m_ttsLang = "none";
    this->m_ttsEnabled = false;
    this->m_flightModeEnabled = false;

    counter_sent = 0;
    counter_received = 0;
    counter_timeout = 0;

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    presetModel = new PresetModel(this);

    battery_timer = new QTimer();
    battery_timer->setInterval(120*1000);
    connect(battery_timer, SIGNAL(timeout()), this, SLOT(updateBatteryLevel()));

    connect(this, SIGNAL(newMessageInQueue()), this, SLOT(manageQueue()), Qt::DirectConnection);

    queue_noblock = new QTimer();
    queue_noblock->setInterval(100);
    connect(queue_noblock, SIGNAL(timeout()), this, SLOT(manageQueue()), Qt::DirectConnection);
    queue_noblock->start();

    answerTimeout_timer = new QTimer();
    answerTimeout_timer->setSingleShot(true);
    answerTimeout_timer->setInterval(4000);
    connect(answerTimeout_timer, SIGNAL(timeout()), &waitForAnswerLoop, SLOT(quit()));

    autoconnection_timer = new QTimer();
    autoconnection_timer->setSingleShot(true);
    autoconnection_timer->setInterval(3000);
    connect(autoconnection_timer, SIGNAL(timeout()), this, SLOT(startAutoConnection()));

    plfPacket_timer = new QTimer();
    plfMutex = new QMutex();

#if defined(Q_OS_LINUX) and !defined(Q_OS_ANDROID)
    dbus = 0;

    mpris_artist = "";
    mpris_album = "";
    mpris_genre = "";
    mpris_song = "";

    mpris_timer = new QTimer();
    mpris_timer->setSingleShot(true);
    mpris_timer->setInterval(5000);
    connect(mpris_timer, SIGNAL(timeout()), this, SLOT(mprisInit()));
    mprisInit();
#endif

    this->m_oldPresetId = 0;

}

void ParrotZikTweeker::setDefaultCapabilities()
{
    int modelNumber;

    modelNumber = m_model == "ZIK1" ? 1 : m_model == "ZIK2" ? 3 : m_model == "ZIK3" ? 5 : 3;

    m_capabilities = 0;

    switch(modelNumber){
        case 5: // zik 3
            m_capabilities |= PresetMultiDevice;
            m_capabilities |= AutoNc;
            m_capabilities |= USBAudio;
            m_capabilities |= USBAudio51;
            m_capabilities |= ConcertHallSurround;
            m_capabilities |= NcDuringCall;
            m_capabilities |= BluetoothDelay;
        case 3: // zik 2
            m_capabilities |= ThumbEqualizer;
            m_capabilities |= Preset;
            m_capabilities |= TTS;
            m_capabilities |= StreetMode;
            m_capabilities |= FlightMode;
        case 1: // zik 1
        default:
            m_capabilities |= Anc;
            m_capabilities |= ConcertHall;
            m_capabilities |= Linein;
            m_capabilities |= BasicSettings;
            m_capabilities |= Telephony;
            break;
    }

    qDebug() << "Setting default capabilities to" << m_capabilities;
    emit capabilitiesChanged();
}

void ParrotZikTweeker::setAlwaysVisible(const bool &always)
{
    if(always != m_alwaysVisible){

        qDebug() << "Setting app as always visible =" << always;
        m_alwaysVisible = always;

        if(m_alwaysVisible == true){
            this->setApplicationVisible(true);
        }
    }
}

void ParrotZikTweeker::setApplicationVisible(bool visible)
{
    if(visible){

        if(true != m_viewVisible){
            qDebug("Visible");

            if(!queue_noblock->isActive()){
                qDebug("Restarting nonblock timer");
                queue_noblock->start();
            }

            if(connection){

            }else{
                if(bluetoothOn() && (!autoconnection_timer->isActive()) && (!demoMode())){
                    qDebug("Not connected. Restarting autoconnection");
                    autoconnection_timer->setInterval(300);
                    autoconnection_timer->start();
                }
            }

            m_viewVisible = true;
            emit viewVisibleChanged();

#if defined(Q_OS_LINUX) and !defined(Q_OS_ANDROID)
            if(dbus == 0){
                qDebug("Restarting MPRIS media player detection after wake up.");
                mprisInit();
            }
#endif
        }
    }else{

        if(m_alwaysVisible == false){
            qDebug("Not visible");

            if(false != m_viewVisible){
                if(queue_noblock->isActive()){
                    qDebug("Stopping nonblock timer");
                    queue_noblock->stop();
                }

                if(connection){

                }else{
                    if(autoconnection_timer->isActive()){
                        qDebug("Stopping autoconnection timer");
                        autoconnection_timer->stop();
                    }
                }

                m_viewVisible = false;
                emit viewVisibleChanged();
            }
        }else{
            qDebug("Main windows not visible, but status locked.");
        }
    }
}

void ParrotZikTweeker::setDemoMode(const bool &enable)
{
    if(enable != m_demoMode){
        m_demoMode = enable;
        emit demoModeChanged();

        if(enable){
            if(autoconnection_timer->isActive()){
                qDebug("Stopping auto-connection in demo mode.");
                autoconnection_timer->stop();
            }

            m_model = "ZIK3";
            emit modelChanged();

            m_battery = "78";
            emit batteryChanged();
            m_batteryState = "charging";
            emit batteryStateChanged();

            m_friendlyName = "My awesome headphones";
            emit friendlyNameChanged();
            m_accountUsername = "Harbour";
            emit accountUsernameChanged();
            m_version = "2.02";
            emit versionChanged();
            m_picVersion = "3";
            emit picVersionChanged();
            m_ttsLang = "en-EN";
            emit ttsLangChanged();
            m_ttsLangHumanReadable = "English";
            emit ttsLangHumanReadableChanged();

            m_concertHall = "true";
            emit concertHallChanged();
            m_roomSize = "jazz";
            emit roomSizeChanged();
            m_spatializerAngle = "150";
            emit spatializerAngleChanged();

            m_ncEnabled = "true";
            emit ncEnabledChanged();
            m_ncType = "anc";
            emit ncTypeChanged();
            m_ncValue = "1";
            emit ncValueChanged();
            m_phoneModeNcType = "aoc";
            emit phoneModeNcTypeChanged();
            m_phoneModeNcValue = "1";
            emit phoneModeNcValueChanged();
            m_autoNC = false;
            emit autoNCChanged();


            m_equalizer = "false";
            emit equalizerChanged();
            m_thumb = QPoint(75,225);
            emit thumbChanged();

            m_smartAudioTune = "false";
            emit smartAudioTuneChanged();
            m_presetBypass = "false";
            emit presetBypassChanged();
            m_presetCounter = "10";
            emit presetCounterChanged();
            m_currentId = 0;
            emit currentIdChanged();

#if defined(Q_OS_LINUX) and !defined(Q_OS_ANDROID)
            if(dbus != 0){
                m_artist = mpris_artist;
                m_album = mpris_album;
                m_song = mpris_song;
                m_genre = mpris_genre;
                m_playing = mpris_playing;
            }else{
                m_artist = "";
                m_album = "";
                m_song = "";
                m_genre = "";
                m_playing = false;
            }
#endif

            m_source = "a2dp";
            emit sourceChanged();

            m_internalNoise = "-1";
            emit internalNoiseChanged();
            m_externalNoise = "-1";
            emit externalNoiseChanged();

            m_ttsEnabled = true;
            emit ttsEnabledChanged();

            m_flightModeEnabled = false;
            emit flightModeEnabledChanged();

            m_producerMode = false;
            emit producerModeChanged();

            m_color = "black";
            emit colorChanged();
            m_texture = "leather";
            emit textureChanged();

            m_a2dpDelay = 80;
            emit a2dpDelayChanged();

            setDefaultCapabilities();

            connection = 1;
            emit connected();
        }else{
            connection = 0;
            emit this->disconnected();

            if(!autoconnection_timer->isActive()){
                startAutoConnection();
            }
        }
    }
}

void ParrotZikTweeker::setBluetoothOn(const bool &enable)
{
    if(enable != m_bluetoothOn){
        localDevice->powerOn();
        //localDevice.setHostMode(QBluetoothLocalDevice::HostConnectable);

        m_bluetoothOn = enable;
        emit bluetoothOnChanged();
    }
}

int ParrotZikTweeker::generateRandom(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

void ParrotZikTweeker::mprisInit()
{
#if defined(Q_OS_LINUX) and !defined(Q_OS_ANDROID)
    QStringList players = discoveredMprisPlayer();

    if(players.length() > 0){
        qDebug() << "MPRIS compactible media player found. DBus interface is :" << players[0];
        dbus = new QDBusInterface(players[0], "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties");

        //metadata_timer = new QTimer();
        //metadata_timer->setInterval(100);
        //connect(metadata_timer, SIGNAL(timeout()), this, SLOT(metadataCallback()));
        //metadata_timer->start();

        dbus->connection().connect(players[0],  "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(metadataCallback(QString,QVariantMap,QStringList)));
        getMprisMetadata(); // for first time init
    }else{
        if(viewVisible()){
            qDebug("No MPRIS compactible player found. Looking again in 5sec.");
            mpris_timer->start();
        }else{
            qDebug("No MPRIS compactible player found. Application not visible anymore, stopping lookup.");
        }
    }
#endif
}


QStringList ParrotZikTweeker::discoveredMprisPlayer(){
#if defined(Q_OS_LINUX) and !defined(Q_OS_ANDROID)
    QRegExp rx("org.mpris.MediaPlayer2.*");
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface dbus_iface("org.freedesktop.DBus", "/org/freedesktop/DBus",
                              "org.freedesktop.DBus", bus);
    QStringList list=dbus_iface.call("ListNames").arguments().at(0).toStringList().filter(rx);

    //XXX Remove vlc entries specific to processes
    foreach(QString str, list){
        if(str.contains(QRegExp("org.mpris.MediaPlayer2.vlc-[0-9]*"))){
            list.removeOne(str);
        }
    }
    return(list);
#else
    return QStringList();
#endif
}

void ParrotZikTweeker::metadataCallback(QString dbusinterface,QVariantMap map,QStringList l)
{
    int updateRequired = 0;
#if defined(Q_OS_LINUX) and !defined(Q_OS_ANDROID)

    if(map.contains("PlaybackStatus")){
        qDebug() << "Playback status=" << map.value("PlaybackStatus").toString();

        bool new_playing = map.value("PlaybackStatus").toString() == "Playing" ? true : false;

        if(new_playing != mpris_playing){
            mpris_playing = new_playing;
            updateRequired = 1;
        }
    }

    if(map.contains("Metadata")){

        QVariant v = map.value("Metadata");
        QDBusArgument arg = v.value<QDBusArgument>();

        QVariantMap metaMap;
        arg>>metaMap;

        QString new_song  = metaMap["xesam:title"].toString();
        QString new_artist = metaMap["xesam:artist"].toString();
        QString new_album  = metaMap["xesam:album"].toString();
        QString new_artUrl = metaMap["mpris:artUrl"].toString();
        QStringList genres = metaMap["xesam:genre"].toStringList();
        QString new_genre = "";

        if(genres.size() > 0){
            new_genre = genres[0];
        }

        if(mpris_artist != new_artist){
            mpris_artist = new_artist;
            updateRequired = 1;
        }

        if(mpris_album != new_album){
            mpris_album = new_album;
            updateRequired = 1;
        }

        if(mpris_song != new_song){
            mpris_song = new_song;
            updateRequired = 1;
        }

        if(mpris_genre != new_genre){
            mpris_genre = new_genre;
            updateRequired = 1;
        }

        if(mpris_artUrl != new_artUrl){
            mpris_artUrl = new_artUrl;
            emit artUrlChanged();
        }

   }

    if(updateRequired){
        qDebug() << mpris_artist << " - " << mpris_album << " - " << mpris_song << " - " << mpris_genre;

        if(demoMode() || (model() == "ZIK1")){

            if(m_artist != mpris_artist) { m_artist = mpris_artist; emit artistChanged(); }
            if(m_album != mpris_album) { m_album = mpris_album; emit albumChanged(); }
            if(m_song != mpris_song) { m_song = mpris_song; emit songChanged(); }
            if(m_genre != mpris_genre) { m_genre = mpris_genre; emit genreChanged(); }
            if(m_playing != mpris_playing) { m_playing = mpris_playing; emit playingChanged(); }

            emit metadataChanged();
        }else{
            this->forceMetadata(mpris_artist, mpris_album, mpris_song, mpris_genre);
        }
    }


#endif

}


void ParrotZikTweeker::getMprisMetadata(bool forceUpdate)
{
    int updateRequired = 0;

    if(forceUpdate) updateRequired = 1;

#if defined(Q_OS_LINUX) and !defined(Q_OS_ANDROID)

    if(dbus != 0){

    qDebug("Getting MPRIS metadata !");


        QDBusReply<QVariant> metaVar = dbus->call("Get","org.mpris.MediaPlayer2.Player","Metadata");
        QDBusError err = dbus->lastError();
        if(!err.isValid()){

            QDBusArgument arg = metaVar.value().value<QDBusArgument>();
            QVariantMap metaMap;
            arg>>metaMap;
            QStringList output;
            QString new_song  = metaMap["xesam:title"].toString();
            QString new_artist = metaMap["xesam:artist"].toString();
            QString new_album  = metaMap["xesam:album"].toString();
            QString new_artUrl = metaMap["mpris:artUrl"].toString();
            QStringList genres = metaMap["xesam:genre"].toStringList();
            QString new_genre = "";
            bool new_playing;

            if(genres.size() > 0){
                new_genre = genres[0];
            }

            if(mpris_artist != new_artist){
                mpris_artist = new_artist;
                updateRequired = 1;
            }

            if(mpris_album != new_album){
                mpris_album = new_album;
                updateRequired = 1;
            }

            if(mpris_song != new_song){
                mpris_song = new_song;
                updateRequired = 1;
            }

            if(mpris_genre != new_genre){
                mpris_genre = new_genre;
                updateRequired = 1;
            }

            if(mpris_artUrl != new_artUrl){
                mpris_artUrl = new_artUrl;
                emit artUrlChanged();
            }

            QDBusReply<QVariant> metaVar2 = dbus->call("Get","org.mpris.MediaPlayer2.Player","PlaybackStatus");
            QDBusError err = dbus->lastError();
            if(!err.isValid()){
                QVariant var = metaVar2.value();
                new_playing = var.toString() == "Playing" ? true : false;

                if(new_playing != mpris_playing){
                    mpris_playing = new_playing;
                    updateRequired = 1;
                }
            }

            if(updateRequired){
                qDebug() << mpris_artist << " - " << mpris_album << " - " << mpris_song << " - " << mpris_genre;

                if(demoMode()){
                    qDebug("Forcing demo mode");

                    if(m_artist != mpris_artist) { m_artist = mpris_artist; emit artistChanged(); }
                    if(m_album != mpris_album) { m_album = mpris_album; emit albumChanged(); }
                    if(m_song != mpris_song) { m_song = mpris_song; emit songChanged(); }
                    if(m_genre != mpris_genre) { m_genre = mpris_genre; emit genreChanged(); }
                    if(m_playing != mpris_playing) { m_playing = mpris_playing; emit playingChanged(); }

                    emit metadataChanged();
                }else{
                    this->forceMetadata(mpris_artist, mpris_album, mpris_song, mpris_genre);
                }
            }

        }else{
            qDebug() << "MPRIS media player disappeared. Starting discovery.";
            dbus = 0;
            //metadata_timer->stop();
            mpris_timer->start();
        }

    }
#endif

}

void ParrotZikTweeker::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    qDebug() << "Found new device:" << device.name() << '(' << device.address().toString() << ')';

    if(device.address().toString().toLower().startsWith("90:03:B7") || device.address().toString().toLower().startsWith("a0:14:3d") || device.address().toString().toLower().startsWith("f0:18:e5:9f") || device.address().toString().toLower().startsWith("88:82:b4")){
        discoveryAgent->stop();

        qDebug("Product seems to be a parrot product. Scanning services");

#if defined(Q_OS_WINRT) or defined(Q_OS_ANDROID)
        // No need to scan services, already included in WINRT device discovery.
        // TODO: cleanup a bit and reuse "serviceDiscovered"

        for(int i=0; i<device.serviceUuids().length(); i++){
            QBluetoothUuid uuid = device.serviceUuids().at(i);
            QString uuid_str = uuid.toString();

            if(uuid_str == "{8b6814d3-6ce7-4498-9700-9312c1711f63}"){
                qDebug() << "Found ZIK 2.0 service (" << uuid_str << "). Connecting.";

                QBluetoothServiceInfo service;
                service.setServiceUuid(uuid);
                service.setDevice(device);

                if(m_model != "ZIK2"){
                    m_model = "ZIK2";
                    modelChanged();
                    setDefaultCapabilities();
                }

				discoveryAgent->stop();
                startClient(service);
                //discoveryAgent->stop();

            }else if(uuid_str == "{8b6814d3-6ce7-4498-9700-9312c1711f64}"){
                qDebug() << "Found Zik 3 service (" << uuid_str << "). Connecting.";

                QBluetoothServiceInfo service;
                service.setServiceUuid(uuid);
                service.setDevice(device);

                if(m_model != "ZIK3"){
                    m_model = "ZIK3";
                    modelChanged();
                    setDefaultCapabilities();
                }

				discoveryAgent->stop();
                startClient(service);
                //discoveryAgent->stop();

            }else if(uuid_str == "{0ef0f502-f0ee-46c9-986c-54ed027807fb}"){
                qDebug() << "Found ZIK 1 service (" << uuid_str << "). Connecting.";

                QBluetoothServiceInfo service;
                service.setServiceUuid(uuid);
                service.setDevice(device);

                if(m_model != "ZIK1"){
                    m_model = "ZIK1";
                    modelChanged();
                    setDefaultCapabilities();
                }

				discoveryAgent->stop();
                startClient(service);
                //discoveryAgent->stop();

            }else{
                qDebug("No Zik service on device, skipping");
            }
        }

#else

        if(!serviceDiscoveryAgent) {
            serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(device.address());
            connect(serviceDiscoveryAgent, SIGNAL(serviceDiscovered(const QBluetoothServiceInfo&)), this, SLOT(serviceDiscovered(const QBluetoothServiceInfo&)));
            connect(serviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(serviceDiscoveryFinished()));
        }else{
            qDebug("Stopping current service discovery");
            serviceDiscoveryAgent->stop();
        }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)) || defined(QTBLUETOOTH_BACKPORT)
        serviceDiscoveryAgent->setRemoteAddress(device.address());
#else
        serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(device.address());
#endif

        serviceDiscoveryAgent->start(QBluetoothServiceDiscoveryAgent::MinimalDiscovery);
#endif
    }
}

void ParrotZikTweeker::deviceDiscoveryFinished()
{
    if(socket){
        if(socket->state() == QBluetoothSocket::UnconnectedState){
            qDebug() << "Device discovery finished and we're not connecting meaning... Zik NOT found, retrying in " << autoconnection_timer->interval() << "ms.";
            autoconnection_timer->setInterval(3000);
            autoconnection_timer->start();
        }
	}else {
		qDebug() << "Device discovery finished and socket not created meaning... Zik NOT found, retrying in " << autoconnection_timer->interval() << "ms.";
		autoconnection_timer->setInterval(3000);
		autoconnection_timer->start();
	}
}

void ParrotZikTweeker::serviceDiscovered(const QBluetoothServiceInfo &service)
{

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)) || defined(QTBLUETOOTH_BACKPORT)

    QString uuid;

    if(service.serviceClassUuids().length()){
        uuid = service.serviceClassUuids().at(0).toString();
    }else{
        uuid = service.serviceUuid().toString();
    }


    if(uuid == "{8b6814d3-6ce7-4498-9700-9312c1711f63}"){
        qDebug() << "Found ZIK 2.0 service (" << service.serviceName() << uuid << "). Connecting.";
#if !defined(Q_OS_OSX) && !defined(Q_OS_ANDROID)	// Do NOT stop agent on OSX in qt 5.5 <= beta (Crash).
       serviceDiscoveryAgent->stop();
#endif

        if(m_model != "ZIK2"){
            m_model = "ZIK2";
            modelChanged();
            setDefaultCapabilities();
        }

        startClient(service);

    }else if(uuid == "{8b6814d3-6ce7-4498-9700-9312c1711f64}"){
        qDebug() << "Found Zik 3 service (" << service.serviceName() << uuid << "). Connecting.";
#if !defined(Q_OS_OSX) && !defined(Q_OS_ANDROID)	// Do NOT stop agent on OSX in qt 5.5 <= beta (Crash).
        serviceDiscoveryAgent->stop();
#endif

        if(m_model != "ZIK3"){
            m_model = "ZIK3";
            modelChanged();
            setDefaultCapabilities();
        }

        startClient(service);

    }/*else if(uuid == "{8b6814d3-6ce7-4498-9700-9312c1711f65}"){
        qDebug() << "Found Zik Sport service (" << service.serviceName() << uuid << "). Connecting.";
#if !defined(Q_OS_OSX) && !defined(Q_OS_ANDROID)	// Do NOT stop agent on OSX in qt 5.5 <= beta (Crash).
        serviceDiscoveryAgent->stop();
#endif

        if(m_model != "ZIKSPORT"){
            m_model = "ZIKSPORT";
            modelChanged();
            setDefaultCapabilities();
        }

        startClient(service);

    }*/else if(uuid == "{0ef0f502-f0ee-46c9-986c-54ed027807fb}"){
        qDebug() << "Found ZIK 1 service (" << service.serviceName() << uuid << "). Connecting.";
#if !defined(Q_OS_OSX) && !defined(Q_OS_ANDROID)	// Do NOT stop agent on OSX in qt 5.5 <= beta (Crash).
        serviceDiscoveryAgent->stop();
#endif

        if(m_model != "ZIK1"){
            m_model = "ZIK1";
            modelChanged();
            setDefaultCapabilities();
        }

        startClient(service);
    }else{
        qDebug() << "No Zik service on device, skipping (service " << uuid << ")";
    }
#else
    //qDebug() << "Found new service:" << service.serviceName();

    if(service.serviceName() == "Parrot RFcomm service"){
        qDebug() << "Found Parrot RFcomm service. Connecting.";
        serviceDiscoveryAgent->stop();

        if(m_model != "invalid"){
            m_model = "invalid";
            setDefaultCapabilities();
        }

        startClient(service);
    }

#endif

}

void ParrotZikTweeker::serviceDiscoveryFinished()
{
    // Deleting the agent ensure old discovered service are taken into account again when the object is automatically recreated.
    delete serviceDiscoveryAgent;
    serviceDiscoveryAgent = 0;

    if(socket){
        if(socket->state() == QBluetoothSocket::ConnectedState){
             qDebug() << "Service discovery finished. Connected.";
        }else if(socket->state() == QBluetoothSocket::ConnectingState){
             qDebug() << "Service discovery finished. Connecting to (apparently) found service.";
        }

    }
}

ParrotZikTweeker::~ParrotZikTweeker()
{
	closeBTConnectionsBeforeQuit();
}

void ParrotZikTweeker::setCapabilities(const Capabilities &capa)
{
    if(m_capabilities != capa){
        m_capabilities = capa;
        emit capabilitiesChanged();
    }
}

void ParrotZikTweeker::startClient(const QBluetoothServiceInfo &remoteService)
{
    if(!socket){
        qDebug() << "Non existant socket, creating it and connecting different signals";

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)) || defined(QTBLUETOOTH_BACKPORT)
        socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
#else
        socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);
#endif
        this->connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
        this->connect(socket, SIGNAL(disconnected()), this, SLOT(stopClient()));
        this->connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(socketError(QBluetoothSocket::SocketError)));
        this->connect(socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)), this, SLOT(socketStateChanged(QBluetoothSocket::SocketState)));
        this->connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));

    }

    if(socket->state() != QBluetoothSocket::UnconnectedState){
        qDebug("Socket busy : connection in progress or already connected.");
        return;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)) || defined(QTBLUETOOTH_BACKPORT)
    QBluetoothUuid uuid;
    QString uuid_str;


    if(remoteService.serviceClassUuids().length()){
        uuid = remoteService.serviceClassUuids().at(0);
        uuid_str = remoteService.serviceClassUuids().at(0).toString();
    }else{
        uuid = remoteService.serviceUuid();
        uuid_str = remoteService.serviceUuid().toString();
    }

    qDebug() << "Connecting to service" << remoteService.serviceName() << "(" << uuid_str << ") on"
             << remoteService.device().name() << "(" << remoteService.device().address().toString() << ")";
    socket->connectToService(remoteService.device().address(), uuid);
#else
    qDebug() << "QtBluetooth < 5.2.0 : Compat mode. Connecting to service" << remoteService.serviceName() << "on" << remoteService.device().name() << "(" << remoteService.device().address().toString() << ")";
    //socket->connectToService(remoteService.device().address(), remoteService.serviceClassUuids().at(0));
    socket->connectToService(remoteService);
#endif


}

int ParrotZikTweeker::isConnected()
{
    return this->connection;
}

void ParrotZikTweeker::socketConnected()
{
    qDebug() << "Connected";


    QByteArray init;
    init.append((char) 0);
    init.append(3);
    init.append((char) 0);

    socket->write(init);

    waitForAnswerLoop.exec();
    this->connection = 1;

    if(model() == "ZIK1"){
        updateBatteryLevel();
        updateFriendlyName();
        updateVersion();
        updateRoomSize();
        updateSpatializerAngle();
        updateAnc();
        updateConcertHall();
        updateEqualizer();
        updateOldPresetId();
        updateHeadDetectionEnabled();
        updateAutoConnectionEnabled();
        updateAutoPowerOff();

        // Cannot get source information on ZIK1. Forcing A2DP for metadata panel
        m_source = "a2dp";
        emit sourceChanged();
    }else if( (model() == "ZIK2") || (model() == "ZIK3") ){
        setAppliVersion("1.4");
        updateBatteryLevel();
        updateFriendlyName();
        updateVersion();
        updateAllSpatialisation();
        updateAnc();
        updateNcEnabled();
        updateNcType();
        updateEqualizer();
        updateThumb();
        updateMetadata();
        updateNoise();
        updateSmartAudioTune();
        updatePresetBypass();
        updateTtsEnabled();
        updateFlightModeEnabled();
        updateHeadDetectionEnabled();
        updateAutoConnectionEnabled();
        updateSource();
        updateAutoPowerOff();

        m_color = "black";
        m_texture = "leather";
        emit colorChanged();
        emit textureChanged();

        updateColor();
        updateTexture();

        updateCapabilities();

        updateA2dpDelay();

        if(model() == "ZIK3"){
            updateParametricEqualizerValues();
            updatePINumber();
        }

        updateAccountUsername();
        updatePresetCounter();
        presetModel->initPresets(accountUsername(), presetCounter().toInt());
        updateCurrentPreset();

        battery_timer->start();

        getMprisMetadata(true);
    }else if(model() == "invalid"){
        qDebug("invalid model ! oh yeah !");
    }

    emit connected();
}

static QString xmlSlice;
void ParrotZikTweeker::readSocket()
 {
    //qDebug("Read Socket");
    //if (!socket)
    //    return;
    bool answer = false;

    if(this->connection == 0){
        QByteArray initAnswer = socket->read(3);
        qDebug() << "Init message received : " << initAnswer.length();
        this->connection = 1;
        emit answerReceived();
        if(!socket->bytesAvailable()) return;
    }

    QByteArray line = socket->readAll();
    //qDebug() << "message received (" << line.length() << "): " << QString(line);

    if(m_downloadInProgress){

        int packetId = 0;

        //qDebug() << "Received packet. Size " << line.size();

        // Get the control / status byte located at byte 8 (starting from 0)
        char control = line.at(7);

        /*
         * Bytes 9 & 10 are the packet id.
         * The bytes are inverted, we have to put them back in the good order.
         * See line 16 and 17
         */
        unsigned char msb = line.at(8);
        unsigned char lsb = line.at(9);

        packetId = lsb;
        packetId = packetId | (msb << 8);

        if(control == 0x11){ // ACK. Usually only on last packet
            qDebug() << "ACK on packet id " << packetId;
            if(packetId*990 >= plfFile->size()){ // if last packet, download is over.
                qDebug("Download Finished. Post processing.");
                plfFile->close();
            }
        }else if(control == 0x22){ // NAK.
            qDebug() << "NAK on packet id " << packetId;
            qDebug() << "Seeking file at " << packetId * 990;

            // Lock file mutex and thus packet sending and wait 100ms for the headphone to have some respite.
            plfMutex->lock();

#if defined(Q_OS_WINRT)
            //no usleep available on WinRT
#else
            //usleep(100000);
#endif

            // Seek to packet id corresponding position in file
            if(!plfFile->seek((packetId) * 990)) qDebug("seeking failed");

            // unlock the mutex and restart packet sending starting from the NAK packet.
            plfMutex->unlock();
            if(!plfPacket_timer->isActive()){
                plfPacket_timer->start();
            }
        }else if(control == 0x33){ // Can only be received once download is over. Very rare.
            qDebug("Download post processing failed !");
            m_downloadInProgress = false;
        }else if(control == 0x35){ // Can only be received once download is over.
            qDebug("Download post processing OK!");
            m_downloadInProgress = false;
        }

        // one of 0x33 or 0x35 MUST be received at the end of the download.

        return;
    }

    qDebug() << "Header debug:" << line.at(0) << line.at(1) << line.at(2) << line.at(3) << line.at(4) << line.at(5) << line.at(6);
    QString xmltext;

    QXmlStreamReader xml;

    // Is there a previous xml slice ?
    if(xmlSlice.isEmpty()){
        xmltext = QString(line.remove(0,7)); // remove header

        xml.addData(xmltext);
    }else{
        qDebug("Prior xml data slice detected. Appending.");
        xmltext = QString(line); // no header here

        xmlSlice.append(xmltext);
        xml.addData(xmlSlice);
    }

    qDebug() << "Read (" << line.length() << ") : " << xmltext;

    //try to detect errors in xml beforehand
    // Read through all document
    while (!xml.atEnd()) {
        xml.readNext();
    }

    // Look for errors
    if (xml.hasError()) {
        if(xml.error() == QXmlStreamReader::PrematureEndOfDocumentError){
            qDebug() << "XML document not full, considering this a part of a greater evil xml scheme.";
            xmlSlice.append(xmltext);
            return;
        }else{
            qDebug() << "BIG error ! Clearing slice info and so on => " << xml.errorString();
            xmlSlice.clear();
            return;
        }
    }

    // No error detected (i.e. Full xml message). Clear slices and rewind xml streamer object.
    if(xmlSlice.isEmpty()){
        xml.clear();
        xml.addData(xmltext);
    }else{
        qDebug("Spliced XML message successfully recovered. Clearing slices.");
        xml.clear();
        xml.addData(xmlSlice);
        qDebug() << "Full recovered message:" << xmlSlice;
        xmlSlice.clear();
    }

    QString path;
    while (!xml.atEnd()) {

        if (xml.hasError()) {
            qDebug() << "error ! => " << xml.errorString(); // Shouldn't really happen since we checked for errors above... 
            break;
        }

        if(xml.isStartElement()){

            if(xml.name() == "battery"){

                if(model() == "ZIK1"){
                    if(m_battery != xml.attributes().value("level").toString()){
                        qDebug() << "Setting battery level to " << xml.attributes().value("level").toString();

                        if(xml.attributes().value("level").toString() == ""){
                            m_battery = "-1";
                        }else{
                            m_battery = xml.attributes().value("level").toString();
                        }
                        emit batteryChanged();
                    }
                }else{
                    if(m_battery != xml.attributes().value("percent").toString()){
                        qDebug() << "Setting battery level to " << xml.attributes().value("percent").toString();

                        m_battery = xml.attributes().value("percent").toString();
                        emit batteryChanged();
                    }

                    if(m_batteryState != xml.attributes().value("state").toString()){
                        qDebug() << "Setting battery state to " << xml.attributes().value("state").toString();

                        m_batteryState = xml.attributes().value("state").toString();
                        emit batteryStateChanged();
                    }

                }

            }else if(xml.name() == "bluetooth"){

                if(m_friendlyName != xml.attributes().value("friendlyname").toString()){
                    qDebug() << "Setting friendly name to " << xml.attributes().value("friendlyname").toString();

                    m_friendlyName = xml.attributes().value("friendlyname").toString();
                    emit friendlyNameChanged();
                }


            }else if(xml.name() == "account"){

                QString new_username = xml.attributes().value("username").toString();

                if(new_username != m_accountUsername){
                    qDebug() << "Setting Parrot account username to " << new_username;
                    m_accountUsername = new_username;
                    emit accountUsernameChanged();
                }

            }else if(xml.name() == "features"){

                if(xml.attributes().hasAttribute("set1")){

                    Capabilities newCapabilities =  static_cast<Capabilities>(xml.attributes().value("set1").toInt());

                    if(m_capabilities != newCapabilities){
                        qDebug() << "Setting capabilities to " << newCapabilities;

                        m_capabilities = newCapabilities;
                    }


                    if(m_capabilities & Anc){
                        qDebug() << "Anc capable";
                    }
                    if(m_capabilities & ConcertHall ){
                        qDebug() << "ConcertHall capable";
                    }
                    if(m_capabilities & Linein ){
                        qDebug() << "Linein capable";
                    }
                    if(m_capabilities & BasicSettings ){
                        qDebug() << "BasicSettings capable";
                    }
                    if(m_capabilities & ThumbEqualizer ){
                        qDebug() << "ThumbEqualizer capable";
                    }
                    if(m_capabilities & Telephony ){
                        qDebug() << "Telephony capable";
                    }
                    if(m_capabilities & TTS ){
                        qDebug() << "TTS capable";
                    }
                    if(m_capabilities & StreetMode ){
                        qDebug() << "StreetMode capable";
                    }
                    if(m_capabilities & FlightMode ){
                        qDebug() << "FlightMode capable";
                    }
                    if(m_capabilities & Preset ){
                        qDebug() << "Preset capable";
                    }
                    if(m_capabilities & PresetMultiDevice ){
                        qDebug() << "PresetMultiDevice capable";
                    }
                    if(m_capabilities & AutoNc ){
                        qDebug() << "AutoNc capable";
                    }
                    if(m_capabilities & NcDuringCall ){
                        qDebug() << "NcDuringCall capable";
                    }
                    if(m_capabilities & USBAudio ){
                        qDebug() << "USBAudio capable";
                    }
                    if(m_capabilities & USBAudio51 ){
                        qDebug() << "USBAudio51 capable";
                    }
                    if(m_capabilities & ConcertHallSurround ){
                        qDebug() << "ConcertHallSurround capable";
                    }
                    if(m_capabilities & BluetoothDelay ){
                        qDebug() << "BluetoothDelay capable";
                    }
                    if(m_capabilities & BluetoothMultiPoint ){
                        qDebug() << "BluetoothMultiPoint capable";
                    }

                }

            }else if(xml.name() == "color"){

                if(xml.attributes().hasAttribute("value")){

                    QString c = "black";
                    int value = xml.attributes().value("value").toInt();

                    switch(value){
                        case 1:
                            c = "black"; break;
                        case 2:
                            c = "blue"; break;
                        case 3:
                            c = "brown"; break;
                        case 4:
                            c = "orange"; break;
                        case 5:
                            c = "white"; break;
                        case 6:
                            c = "yellow"; break;
                        case 7:
                            c = "green"; break;
                        case 8:
                            c = "red"; break;
                        case 9:
                            c = "ivory"; break;
                        case 10:
                            c = "camel"; break;
                        default:
                            c = "black"; break;
                    }

                    if(m_color != c){
                        qDebug() << "Setting headphones color to " << c;
                        m_color = c;
                        emit colorChanged();
                    }else{
                        qDebug() << "Headphones color is already set to " << c;
                    }
                }

            }else if(xml.name() == "texture"){

                if(xml.attributes().hasAttribute("value")){

                    QString t = "leather";
                    int value = xml.attributes().value("value").toInt();

                    switch(value){
                        case 1:
                            t = "leather"; break;
                        case 2:
                            t = "crocodile"; break;
                        case 3:
                            t = "overstitched"; break;
                        default:
                            t = "leather"; break;
                    }

                    if(m_texture != t){
                        qDebug() << "Setting headphones texture to " << t;
                        m_texture = t;
                        emit textureChanged();
                    }else{
                        qDebug() << "Headphones texture is already set to " << t;
                    }
                }

            }else if(xml.name() == "software"){

                //qDebug() << "Setting version to " << xml.attributes().value("version").toString();
                //QString newVersion = xml.attributes().value("version").toString();

                /*if(path == "/api/software/version/sip6/get"){
                    if(m_version != newVersion){
                        qDebug() << "Setting SIP6 version to " << newVersion;
                        m_version = newVersion;
                        emit versionChanged();
                    }
                }else if(path == "/api/software/version/pic/get"){
                    if(m_picVersion != newVersion){
                        qDebug() << "Setting PIC version to " << newVersion;
                        m_picVersion = newVersion;
                        emit picVersionChanged();
                    }
                }*/

                if(xml.attributes().hasAttribute("version")){	// ZIK 1
                    QString v = xml.attributes().value("version").toString();

                    qDebug() << "Setting SIP6 version to " << v;
                    m_version = v;
                }

                if(xml.attributes().hasAttribute("sip6")){
                    QString v = xml.attributes().value("sip6").toString();

                    qDebug() << "Setting SIP6 version to " << v;
                    m_version = v;
                }

                if(xml.attributes().hasAttribute("pic")){
                    QString v = xml.attributes().value("pic").toString();

                    qDebug() << "Setting PIC version to " << v;
                    m_picVersion = v;
                }

                if(xml.attributes().hasAttribute("tts")){
                    QString v = xml.attributes().value("tts").toString();

                    qDebug() << "Setting TTS lang to " << v;
                    m_ttsLang = v;

                    QString lang;

                    if(m_ttsLang.compare("fr-FR") == 0){
                        lang = "French";
                    }else if(m_ttsLang.compare("en-GB") == 0){
                        lang = "English";
                    }else if(m_ttsLang.compare("es-ES") == 0){
                        lang = "Spanish";
                    }else if(m_ttsLang.compare("it-IT") == 0){
                        lang = "Italian";
                    }else if(m_ttsLang.compare("pt-PT") == 0){
                        lang = "Portuguese";
                    }else if(m_ttsLang.compare("de-DE") == 0){
                        lang = "German";
                    }else if(m_ttsLang.compare("ru-RU") == 0){
                        lang = "Russian";
                    }else if(m_ttsLang.compare("ja-JP") == 0){
                        lang = "Japanese";
                    }else if(m_ttsLang.compare("zh-CN") == 0){
                        lang = "Chinese";
                    }else if(m_ttsLang.compare("tr-TR") == 0){
                        lang = "Turkish";
                    }else if(m_ttsLang.compare("none") == 0){
                        lang = "none";
                    }

                    qDebug() << "Setting TTS lang human readable to " << lang;
                    m_ttsLangHumanReadable = lang;

                    emit ttsLangChanged();
                    emit ttsLangHumanReadableChanged();
                }

                emit versionChanged();

            }else if(xml.name() == "sound_effect"){



                if(xml.attributes().hasAttribute("room_size")){
                    if(m_roomSize != xml.attributes().value("room_size").toString()){
                        qDebug() << "Setting room size to " << xml.attributes().value("room_size").toString();
                        m_roomSize = xml.attributes().value("room_size").toString();
                        emit roomSizeChanged();
                    }
                }

                if(xml.attributes().hasAttribute("angle")){
                    if(m_spatializerAngle != xml.attributes().value("angle").toString()){
                        qDebug() << "Setting angle to " << xml.attributes().value("angle").toString();
                        m_spatializerAngle = xml.attributes().value("angle").toString();
                        emit spatializerAngleChanged();
                    }
                }

                if(xml.attributes().hasAttribute("enabled")){
                    if(m_concertHall != xml.attributes().value("enabled").toString()){
                        qDebug() << "Setting concertHall to " << xml.attributes().value("enabled").toString();
                        m_concertHall = xml.attributes().value("enabled").toString();
                        emit concertHallChanged();
                    }
                }

                if(xml.attributes().hasAttribute("mode")){
                    QString mode = xml.attributes().value("mode").toString();
                    bool surround = (mode == "surround") ? true : false;

                    qDebug() << "surroundMode is " << surround;

                    if(m_surroundMode != surround){
                        qDebug() << "Setting surroundMode to " << surround;
                        m_surroundMode = surround;
                        emit surroundModeChanged();
                    }
                }else{
                    qDebug() << "Surround mode does not seems to be supported. Setting surroundMode to false";
                }

            }else if(xml.name() == "noise_cancellation"){

                qDebug() << "Setting ANC to " << xml.attributes().value("enabled").toString();

                if(m_anc != xml.attributes().value("enabled").toString()){
                    m_anc = xml.attributes().value("enabled").toString();
                    emit ancChanged();
                }


            }else if(xml.name() == "noise_control"){

                if(xml.attributes().hasAttribute("enabled")){
                    if(m_ncEnabled != xml.attributes().value("enabled").toString()){
                        qDebug() << "Setting ncEnabled to " << xml.attributes().value("enabled").toString();
                        m_ncEnabled = xml.attributes().value("enabled").toString();
                        emit ncEnabledChanged();
                    }
                }

                if(xml.attributes().hasAttribute("type") && xml.attributes().hasAttribute("value")){
                        qDebug() << "NC type = " << xml.attributes().value("type").toString();
                        m_ncType = xml.attributes().value("type").toString();

                        qDebug() << "NC value = " << xml.attributes().value("value").toString();
                        m_ncValue = xml.attributes().value("value").toString();

                        emit ncTypeChanged();
                        emit ncValueChanged();
                }

                if(xml.attributes().hasAttribute("auto_nc")){
                        QString autoNC_str = xml.attributes().value("auto_nc").toString();

                        bool autoNC;

                        if(autoNC_str == "true"){
                            autoNC = true;
                        }else if(autoNC_str == "false"){
                            autoNC = false;
                        }else{
                            qDebug() << "Unrecognised autoNC state. Ignoring.";
                            autoNC = m_autoNC;
                        }

                        if(m_autoNC != autoNC){
                            qDebug() << "New auto noise control state = " << (autoNC ? "true": "false");
                            m_autoNC = autoNC;
                            emit autoNCChanged();
                        }
                }

            }else if(xml.name() == "equalizer"){

                if(path == "/api/audio/equalizer/preset_id/get"){ // old preset id (zik 1)
                    if(m_oldPresetId != xml.attributes().value("preset_id").toInt()){
                        qDebug() << "Setting old equalizer preset id to " << xml.attributes().value("preset_id").toString();

                        m_oldPresetId = xml.attributes().value("preset_id").toInt();
                        emit oldPresetIdChanged();
                    }
                }else{
                    if(m_equalizer != xml.attributes().value("enabled").toString()){
                        qDebug() << "Setting equalizer to " << xml.attributes().value("enabled").toString();

                        m_equalizer = xml.attributes().value("enabled").toString();
                        emit equalizerChanged();
                    }
                }


            }else if(xml.name() == "tts"){

                qDebug() << "Setting ttsEnabled to " << xml.attributes().value("enabled").toString();

                bool enabled;

                QString status = xml.attributes().value("enabled").toString();
                if(status == "true"){
                    enabled = true;
                }else{
                    enabled = false;
                }

                if(m_ttsEnabled != enabled){
                    m_ttsEnabled = enabled;
                    emit ttsEnabledChanged();
                }

            }else if(xml.name() == "flight_mode"){

                qDebug() << "Setting flightModeEnabled to " << xml.attributes().value("enabled").toString();

                bool enabled;

                QString status = xml.attributes().value("enabled").toString();
                if(status == "true"){
                    enabled = true;
                }else{
                    enabled = false;
                }

                if(m_flightModeEnabled != enabled){
                    m_flightModeEnabled = enabled;
                    emit flightModeEnabledChanged();
                }

            }else if(xml.name() == "head_detection"){

                if(xml.attributes().hasAttribute("enabled")){
                    qDebug() << "Setting headDetectionEnabled to " << xml.attributes().value("enabled").toString();

                    bool enabled;

                    QString status = xml.attributes().value("enabled").toString();
                    if(status == "true"){
                        enabled = true;
                    }else{
                        enabled = false;
                    }

                    if(m_headDetectionEnabled != enabled){
                        m_headDetectionEnabled = enabled;
                        emit headDetectionEnabledChanged();
                    }
                }

            }else if(xml.name() == "auto_connection"){

                if(xml.attributes().hasAttribute("enabled")){
                    qDebug() << "Setting autoConnectionEnabled to " << xml.attributes().value("enabled").toString();

                    bool enabled;

                    QString status = xml.attributes().value("enabled").toString();
                    if(status == "true"){
                        enabled = true;
                    }else{
                        enabled = false;
                    }

                    if(m_autoConnectionEnabled != enabled){
                        m_autoConnectionEnabled = enabled;
                        emit autoConnectionEnabledChanged();
                    }
                }
            }else if(xml.name() == "preset"){

                if(xml.attributes().hasAttribute("counter")){
                    qDebug() << "Setting Preset counter to " << xml.attributes().value("counter").toString();

                    if(m_presetCounter != xml.attributes().value("counter").toString()){
                        m_presetCounter = xml.attributes().value("counter").toString();
                        emit presetCounterChanged();
                        presetModel->setCounter(m_presetCounter.toInt());
                    }

                }

                if(xml.attributes().hasAttribute("bypass")){
                    qDebug() << "Setting Preset bypass to " << xml.attributes().value("bypass").toString();

                    if(m_presetBypass != xml.attributes().value("bypass").toString()){
                        m_presetBypass = xml.attributes().value("bypass").toString();
                        emit presetBypassChanged();
                    }

                }

                if(xml.attributes().hasAttribute("id")){

                    if(path == "/api/audio/preset/current/get"){
                        qDebug() << "Current Preset changed. New id=" << xml.attributes().value("id").toString();
                        presetModel->setCurrentPreset(xml.attributes().value("id").toInt());

                        m_currentId = xml.attributes().value("id").toInt();
                        emit currentIdChanged();
                    }else{
                        qDebug() << "Saving Preset using id " << xml.attributes().value("id").toString();
                        presetModel->confirmPresetInsertion(xml.attributes().value("id").toInt());
                    }

                }

                if(xml.attributes().hasAttribute("base64")){
                    qDebug() << "AAL Preset detected => " << xml.attributes().value("base64").toString();
                    qDebug() << "AAL Preset base64 length is" << xml.attributes().value("base64").toString().length();
                }


            }else if(xml.name() == "thumb_equalizer"){

                qDebug() << "Setting Thumb radius and theta to " << xml.attributes().value("r").toString() << " and " << xml.attributes().value("theta").toString();

                QPoint p;

                p.setX(xml.attributes().value("r").toInt());
                p.setY(xml.attributes().value("theta").toInt());

                if(m_thumb != p){
                    m_thumb = p;
                    emit thumbChanged();
                }


            }else if(xml.name() == "metadata"){

                QString newArtist = xml.attributes().value("artist").toString();
                QString newAlbum = xml.attributes().value("album").toString();
                QString newSong = xml.attributes().value("title").toString();
                QString newGenre = xml.attributes().value("genre").toString();

                bool newPlaying = xml.attributes().value("playing").toString() == "true" ? true : false;

                if(m_artist != newArtist) { m_artist = newArtist; emit artistChanged(); }
                if(m_album != newAlbum) { m_album = newAlbum; emit albumChanged(); }
                if(m_song != newSong) { m_song = newSong; emit songChanged(); }
                if(m_genre != newGenre) { m_genre = newGenre; emit genreChanged(); }

                if(m_playing != newPlaying) { m_playing = newPlaying; emit playingChanged(); }

                emit metadataChanged();

            }else if(xml.name() == "source"){

                if(xml.attributes().hasAttribute("type")){
                    QString src = xml.attributes().value("type").toString();

                    qDebug() << "New source type: " << src;
                    if(m_source != src){
                        m_source = src;
                        emit sourceChanged();
                    }
                }

            }else if(xml.name() == "noise"){

                if(xml.attributes().hasAttribute("internal")){
                    qDebug() << "Setting internal noise to " << xml.attributes().value("internal").toString();

                    if(m_internalNoise != xml.attributes().value("internal").toString()){
                        m_internalNoise = xml.attributes().value("internal").toString();
                        emit internalNoiseChanged();
                        emit noiseChanged();
                    }

                }

                if(xml.attributes().hasAttribute("external")){
                    qDebug() << "Setting external noise to " << xml.attributes().value("external").toString();

                    if(m_externalNoise != xml.attributes().value("external").toString()){
                        m_externalNoise = xml.attributes().value("external").toString();
                        emit externalNoiseChanged();
                        emit noiseChanged();
                    }

                }

                if(xml.attributes().hasAttribute("reduction")){

                    if(m_noiseReductionLevel != xml.attributes().value("reduction").toInt()){
                        qDebug() << "Setting noise reduction level to " << xml.attributes().value("reduction").toInt();

                        m_noiseReductionLevel = xml.attributes().value("reduction").toInt();
                        emit noiseReductionLevelChanged();
                        emit noiseChanged();
                    }

                }

            }else if(xml.name() == "smart_audio_tune"){
                QString enable = xml.attributes().value("enabled").toString();

                if(enable != m_smartAudioTune){
                    qDebug() << "Setting SmartAudioTune status to " << enable;
                    m_smartAudioTune = enable;
                    emit smartAudioTuneChanged();
                }

            }else if(xml.name() == "delay"){

                if(xml.attributes().hasAttribute("value")){

                    int delay = xml.attributes().value("value").toInt();

                    if(delay != m_a2dpDelay){
                        qDebug() << "Setting A2DP delay to " << delay;
                        m_a2dpDelay = delay;
                        emit a2dpDelayChanged();
                    }

                }

            }else if(xml.name() == "auto_power_off"){

                if(xml.attributes().hasAttribute("value")){

                    int v = xml.attributes().value("value").toInt();

                    if(m_autoPowerOff != v){
                        qDebug() << "Setting auto power off value to" << v;

                        m_autoPowerOff = v;
                        emit autoPowerOffChanged();
                    }
                }

            }else if(xml.name() == "push"){

                qDebug("Data is being pushed !");

            }else if(xml.name() == "notify"){
                QString command = "GET ";
                command.append(xml.attributes().value("path").toString());

                this->sendMessage(command, false);
                //this->thread()->sleep(2);
            }else if(xml.name() == "answer"){

                path = xml.attributes().value("path").toString();
                bool error = false;

                if(xml.attributes().hasAttribute("error")){
                    error = true;
                }

                answer = true;

                if(path == "/api/software/download_size/set"){ // if this is an answer to a firmware download request
                    if(!error){ // and there is no errors
                        qDebug("Download request accepted. Starting download.");
                        m_downloadInProgress = true;
                        sendPlfPacket(); // Start download by sending first packet !
                    }else{
                        qDebug("Error starting download.");
                    }
                }

            }
        }

        xml.readNext();
    }

    qDebug() << "";
    if(answer){
        emit answerReceived();
        counter_received++;
    }

    if (xml.hasError()) {
        qDebug() << "error detected too late : " << xml.errorString();
    }

}

void ParrotZikTweeker::sendMessage(const QString &message, const bool &waitForAnswer)
 {

    if(isConnected() && (!demoMode()) && (!downloadInProgress())){
        qDebug() << "ENQUEUE <= " << message;
        command_queue.enqueue(message);
        emit newMessageInQueue();
    }else if(demoMode()){
        qDebug("Demo mode !");
    }else if(downloadInProgress()){
        qDebug("PLF download in progress. \"Your command\" > /dev/null");
    }
}

void ParrotZikTweeker::manageQueue()
{
    if(!waitForAnswerLoop.isRunning()){
        while(!command_queue.isEmpty()){

            QString message = command_queue.dequeue();

            qDebug() << "SENDING (" << message.toUtf8().length()+3 << ") => " << message;

            QByteArray text;
            //text.append((char) 0); qDebug() << "size " << text.length();
            //text.append(message.toUtf8().length()+3); qDebug() << "size " << text.length();

            int messageLength = message.toUtf8().length()+3;
            quint8 length_b1 = (messageLength & 0xff00) >> 8;
            quint8 length_b2 = (messageLength & 0x00ff);

            /*
             * size header
             */
            text.append(length_b1);
            text.append(length_b2);

            text.append(128); // data type

            text.append(message.toUtf8());

            int len = socket->write(text);
            counter_sent++;

            if(len != text.length()){
                qDebug() << "Message length and sent length are different.";
            }

            //qDebug() << "Waiting for answer";
            //qDebug() << this->thread()->currentThread();
            answerTimeout_timer->start();
            waitForAnswerLoop.exec();
            if(answerTimeout_timer->isActive()){
                answerTimeout_timer->stop();
            }else{
                qDebug() << "TIMEOUT waiting for answer." << " Sent=" << counter_sent << "   Received=" << counter_received << "   Timeout=" << counter_timeout;
                counter_timeout++;
            }

        }
    }
}

void ParrotZikTweeker::sendTestEmbedCommand(const QString &command, const bool &waitForAnswer)
 {
    QString message = QString("GET /testEmbed/exec?arg=").append(command);

    if(isConnected()){
        QByteArray text;
        text.append((char) 0);
        text.append(message.toUtf8().length()+3);
        text.append(128);
        //text.append("\x80");
        text.append(message.toUtf8());
//        text.append(message.toLatin1());

        qDebug()<< message;
        int len = socket->write(text);

        if(len != text.length()){
            qDebug() << "Message length and sent length are different.";
        }


        if(waitForAnswer){
            qDebug() << "Waiting for answer";
            waitForAnswerLoop.exec();
        }
    }
 }

void ParrotZikTweeker::stopClient()
 {
    qDebug("Bluetooth got disconnected. Trying to connect again.");

    if(this->isConnected()){
        connection = 0;
        emit this->disconnected();
    }

    autoconnection_timer->setInterval(3000);
    autoconnection_timer->start();
}

void ParrotZikTweeker::socketError(QBluetoothSocket::SocketError error)
{
    qDebug("Socket error");

    if(socket){
        this->disconnect(socket, 0, 0, 0);

        socket->abort();
        socket->close();
        socket = 0;
    }

    stopClient();
}

void ParrotZikTweeker::socketStateChanged(QBluetoothSocket::SocketState state)
{
    qDebug() << "Socket state changed to" << state;
}

void ParrotZikTweeker::setBattery(const QString &a) {
        if (a != m_battery) {
            m_battery = a;
            emit batteryChanged();
        }
}

void ParrotZikTweeker::setAppliVersion(QString version)
{
    QString command = QString("GET /api/appli_version/set?arg=%1").arg(version);
    this->sendMessage(command);
}

void ParrotZikTweeker::setVersion(const QString &v) {
        if (v != m_version) {
            m_version = v;
            emit versionChanged();
        }
}

void ParrotZikTweeker::setConcertHall(const QString &status)
{
    if (status != m_concertHall) {
        QString command;

        if(model() == "ZIK1"){
            command = QString("GET /api/audio/sound_effect/enabled/set?arg=%1").arg(status);
        }else{
            command = QString("GET /api/audio/sound_effect/enabled/set?arg=%1&producer=%2").arg(status).arg(QString::number(m_producerMode ? 1:0));
        }


        m_concertHall = status;
        emit concertHallChanged();

        this->sendMessage(command);


    }
}

void ParrotZikTweeker::setRoomSize(const QString &s) {
        if (s != m_roomSize) {
            QString command;

            if(model() == "ZIK1"){
                command = QString("GET /api/audio/sound_effect/room_size/set?arg=%1").arg(s);
            }else{
                command = QString("GET /api/audio/sound_effect/room_size/set?arg=%1&producer=%2").arg(s).arg(QString::number(m_producerMode ? 1:0));
            }

            m_roomSize = s;
            emit roomSizeChanged();

            this->sendMessage(command);


        }
}

void ParrotZikTweeker::setSpatializerAngle(const QString &a) {
        if (a != m_spatializerAngle) {
            QString command = QString("GET /api/audio/sound_effect/angle/set?arg=%1&producer=%2").arg(a).arg(QString::number(m_producerMode ? 1:0));

            if(model() == "ZIK1"){
                command = QString("GET /api/audio/sound_effect/angle/set?arg=%1").arg(a);
            }else{
                command = QString("GET /api/audio/sound_effect/angle/set?arg=%1&producer=%2").arg(a).arg(QString::number(m_producerMode ? 1:0));
            }

            m_spatializerAngle = a;
            emit spatializerAngleChanged();

            this->sendMessage(command, true);


        }
}

void ParrotZikTweeker::setAnc(const QString &status)
{
    if (status != m_anc) {
        QString command("GET /api/audio/noise_cancellation/enabled/set?arg=");
        command = command.append(status);

        m_anc = status;
        emit ancChanged();

        this->sendMessage(command);


    }
}

void ParrotZikTweeker::setAoc(QString type, int value)
{
    QString command = QString("GET /api/audio/noise_control/set?arg=%1&value=%2").arg(type).arg(value);
    this->sendMessage(command);

    if(m_ncType != type){
        m_ncType = type;
    }

    if(m_ncValue != QString::number(value)){
        //qDebug() << QString::number(value);
        m_ncValue = QString::number(value);
    }
}

void ParrotZikTweeker::setNcEnabled(const QString &enable)
{
    if (enable != m_ncEnabled) {
        QString command("GET /api/audio/noise_control/enabled/set?arg=");
        command = command.append(enable);

        m_ncEnabled = enable;
        emit ncEnabledChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setAutoNC(const bool &enable)
{
    if (enable != m_autoNC) {
        QString command("GET /api/audio/noise_control/auto_nc/set?arg=");
        command = command.append(enable ? "true" : "false");

        m_autoNC = enable;
        emit autoNCChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setNcType(const QString &type)
{
    if (type != m_ncType) {
        QString command = QString("GET /api/audio/noise_control/set?arg=%1&value=%2").arg(type).arg(m_ncValue);

        m_ncType = type;
        emit ncTypeChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setNcValue(const QString &value)
{
    if (value != m_ncValue) {
        QString command = QString("GET /api/audio/noise_control/set?arg=%1&value=%2").arg(m_ncType).arg(value);

        m_ncValue = value;
        emit ncValueChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setPhoneModeNcValue(const QString &value)
{
    if (value != m_ncValue) {
        QString command = QString("GET /api/audio/noise_control/phone_mode/set?arg=%1&value=%2").arg(m_phoneModeNcType).arg(value);

        m_ncValue = value;
        emit ncValueChanged();

        this->sendMessage(command);
    }
}


void ParrotZikTweeker::setPhoneModeNcType(const QString &type)
{
    if (type != m_ncType) {
        QString command = QString("GET /api/audio/noise_control/phone_mode/set?arg=%1&value=%2").arg(type).arg(m_phoneModeNcValue);

        m_ncType = type;
        emit ncTypeChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setPhoneModeNc(QString type, int value)
{
    QString command = QString("GET /api/audio/noise_control/phone_mode/set?arg=%1&value=%2").arg(type).arg(value);
    this->sendMessage(command);

    if(m_phoneModeNcType != type){
        m_phoneModeNcType = type;
        emit phoneModeNcTypeChanged();
    }

    if(m_phoneModeNcValue != QString::number(value)){
        m_phoneModeNcValue = QString::number(value);
        emit phoneModeNcValueChanged();
    }
}

void ParrotZikTweeker::setEqualizer(const QString &status)
{
    if (status != m_equalizer) {
        QString command("GET /api/audio/equalizer/enabled/set?arg=");
        command = command.append(status);

        m_equalizer = status;
        emit equalizerChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setThumb(const QPoint &p)
{
    if(p != m_thumb){

        m_thumb = p;
        emit thumbChanged();

        float gain1 = thumbToGain[p.x()][p.y()][0];
        float gain2 = thumbToGain[p.x()][p.y()][1];
        float gain3 = thumbToGain[p.x()][p.y()][2];
        float gain4 = thumbToGain[p.x()][p.y()][3];
        float gain5 = thumbToGain[p.x()][p.y()][4];


        QString command = QString("GET /api/audio/thumb_equalizer/value/set?arg=%1,%2,%3,%4,%5,%6,%7").arg(gain1).arg(gain2).arg(gain3).arg(gain4).arg(gain5).arg(p.x()).arg(p.y());

        this->sendMessage(command);

        /*presetModel->setEquaParameters(70, 381, 960, 2419, 11000,
                                      0.3, 0.8, 1, 0.8, 0.3,
                                      gain1, gain2, gain3, gain4, gain5);
        */

    }

}

void ParrotZikTweeker::setOldPresetId(const int &i)
{

    if(m_oldPresetId != i){
        m_oldPresetId = i;

        QString command = QString("GET /api/audio/equalizer/preset_id/set?arg=%1").arg(i);

        this->sendMessage(command);

        emit oldPresetIdChanged();
    }
}

void ParrotZikTweeker::setParametricEqualizer(int freq1, int freq2, int freq3, int freq4, int freq5,
                                            float q1, float q2, float q3, float q4, float q5,
                                            float gain1, float gain2, float gain3, float gain4, float gain5)
{

    QString command = QString("GET /api/audio/param_equalizer/value/set?arg=%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15").arg(freq1).arg(freq2).arg(freq3).arg(freq4).arg(freq5).arg(q1).arg(q2).arg(q3).arg(q4).arg(q5).arg(gain1).arg(gain2).arg(gain3).arg(gain4).arg(gain5);

    this->sendMessage(command);

    presetModel->setEquaParameters(freq1, freq2, freq3, freq4, freq5,
                                  q1, q2, q3, q4, q5,
                                  gain1, gain2, gain3, gain4, gain5);
}

#if defined(Q_OS_LINUX) and !defined(Q_OS_ANDROID)
void ParrotZikTweeker::forceMetadata(QString artist, QString album, QString song, QString genre)
{

    QString command = QString("GET /api/audio/track/metadata/force?artist=%1&album=%2&song=%3&genre=%4").arg(artist.replace("&","\\&")).arg(album.replace("&","\\&")).arg(song.replace("&","\\&")).arg(genre.replace("&","\\&"));

    this->sendMessage(command);
}
#endif

