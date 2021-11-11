#include "parrotziktweaker.h"

void ParrotZikTweeker::sendPlfFile(QString lang)
{

    plfFile = new QFile("/home/lainwir3d/Documents/CubaEvo_3.02_RC1/langs/zik_lang_" + lang + ".plf");
    //plfFile = new QFile("/home/lainwir3d/git/aftermarket/Voyager/Soft/Build/Debug/AutoUpdate_Metros.plf");


    if(plfFile->exists()){
        qDebug() << "Opening plf";

        plfFile->open(QIODevice::ReadOnly);

        QString message = QString("GET /api/software/download_size/set?arg=%1").arg(QString::number(plfFile->size())); // size is in bytes

        QByteArray text;
        text.append((char) 0);
        text.append(message.toUtf8().length()+3);
        text.append(128); //text.append("\x80");

        text.append(message.toUtf8());

        socket->write(text);

        // waiting for headphone to answer or for timeout.
        answerTimeout_timer->start();
        waitForAnswerLoop.exec();

        if(answerTimeout_timer->isActive()){ // if timeout timer is still active, it means the answer has been received.
            answerTimeout_timer->stop();
        }else{ // else a timeout has occured
            qDebug() << "TIMEOUT waiting for answer." << " Sent=" << counter_sent << "   Received=" << counter_received << "   Timeout=" << counter_timeout;
            counter_timeout++;
        }

        // I don't remember why we sleep 1 second, but I think that was before I added the code to check if there was no error starting the transfer
        qDebug("Ready to rumble");

#if defined(Q_OS_WINRT)
        //no sleep available on WinRT
#else
        sleep(1);
#endif

        // Set packet send interval to 6ms, no automatic rearming, and call sendPlfPacket on interval timeout
        plfPacket_timer->setInterval(6);
        plfPacket_timer->setSingleShot(true);
        connect(plfPacket_timer, SIGNAL(timeout()), this, SLOT(sendPlfPacket()));
    }else{
        qDebug() << "File does not exist";
    }
}

void ParrotZikTweeker::sendPlfPacket()
{
    // Lock file access to the other threads
    plfMutex->lock();

    int total = 0;
    int pnumber = 0;

    if(!plfFile->atEnd()) {
        QByteArray packet;

        pnumber = plfFile->pos() / 990; // Compute packet id in a absolute manner. Don't use a counter so that we're always sure of it.

        QByteArray data = plfFile->read(990); // read our 990 bytes

        /*
         * Le paquet comprend 5 parties :
         *   - HEADER : header global du paquet
         *   - DATA HEADER : header de la partie de données du paquet
         *   - DATA : données du fichier
         *   - DATA TRAILER : trailer de la partie de données du paquet
         *
         */

        /*
         * HEADER. Comprend :
         *   - Octet 1 : msb de la longueur TOTALE
         *   - Octet 2 : lsb de la longueur TOTALE
         *   - Octet 3 : 0x80 / 128 (en décimale). Cette valeur est fixe
         */

        // compute total packet size. Equal to size of HEADER (3) + size of DATA HEADER (7) + size of DATA (990 or eventually less for the last packet) + size of DATA TRAILER (2)
        int length = data.length() + 12;
        char lsb = length & 0x000000ff; // isolate size lsb into own byte
        char msb = (length>>8) & 0x000000ff; // isolate size msb into own byte

        // Write HEADER into packet.
        packet.append(msb);
        packet.append(lsb);
        packet.append((char)0x80);

        /*
         * DATA HEADER du paquet de données. Comprend :
         *   - Octet 1 : 1 (fixe)
         *   - Octet 2 : 1 (fixe)
         *   - Octet 3 : msb de la longueur totale du paquet de données
         *   - Octet 4 : lsb de la longueur totale du paquet de données
         *   - Octet 5 : 0 (fixe)
         *   - Octet 6 : msb du numéro du paquet
         *   - Octet 7 : lsb du numéro du paquet
         */

        char ior = 0; // Inclusive OR for DATA TRAILER checksum
        char nor = 0; // Exclusive OR for DATA TRAILER checksum

        // Compute total DATA HEADER + DATA + DATA TRAILER size. Equal to size of DATA HEADER (7) + size of DATA (990 or eventually less for the last packet) + size of DATA TRAILER (2)
        length = data.length()+9;

        packet.append((char) 1); // Write fixed byte 1 of DATA HEADER
        ior = ior | 1; nor = nor ^ 1; // compute inclusive OR and exclusive OR after each byte written.
        packet.append((char) 1); // Write fixed byte 2 of DATA HEADER
        ior = ior | 1; nor = nor ^ 1;


        lsb = length & 0x000000ff; // isolate size lsb into own byte
        msb = (length>>8) & 0x000000ff; // isolate size msb into own byte

        // Write bytes 3 and 4 of the DATA HEADER
        packet.append(msb);
        ior = ior | msb; nor = nor ^ msb;
        packet.append(lsb);
        ior = ior | lsb; nor = nor ^ lsb;

        packet.append((char) 0); // Write fixed byte 5 of DATA HEADER
        ior = ior | 0; nor = nor ^ 0;

        lsb = pnumber & 0x000000ff; // isolate packet id lsb into own byte
        msb = (pnumber>>8) & 0x000000ff; // isolate packet id msb into own byte

        //if((pnumber%50) == 0 )
            qDebug() << "packet id is " << pnumber;

        // Write bytes 6 and 7 of the DATA HEADER
        packet.append(msb);
        ior = ior | msb; nor = nor ^ msb;
        packet.append(lsb);
        ior = ior | lsb; nor = nor ^ lsb;

        /*
         * DATA
         * Données du fichier brutes
         */
        for(int i=0; i<data.length();i++){
            char b = data.at(i);
            ior = ior | b; // compute DATA TRAILER checksum byte by byte
            nor = nor ^ b; // compute DATA TRAILER checksum byte by byte
            packet.append(b); // Write file data to packet byte by byte
        }

        /*
        *  DATA TRAILER. Comprend :
        *  - Octet 1 : OU inclusif de tout les octets du DATA HEADER + DATA + DATA TRAILER
        *  - Octet 2 : OU exclusif de tout les octets du DATA HEADER + DATA + DATA TRAILER
        */
        packet.append(ior); // Write byte 1 of DATA TRAILER
        packet.append(nor); // Write byte 2 of DATA TRAILER

        //QString dbg = QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10").arg(packet.at(0),2,16).arg(packet.at(1),2,16).arg(packet.at(2),2,16).arg(packet.at(3),2,16).arg(packet.at(4),2,16).arg(packet.at(5),2,16).arg(packet.at(6),2,16).arg(packet.at(7),2,16).arg(packet.at(8),2,16).arg(packet.at(9),2,16);
        //qDebug() << dbg;

        //qDebug() << "packet is " << packet.length() << " bytes long";

        int len = socket->write(packet); // Send packet
        //qDebug() << "Wrote " << len << " bytes.";
        total += len;

        if(!plfPacket_timer->isActive()){
            plfPacket_timer->start(); // Restart 6ms timer for next packet scheduling.
        }
    }else{
        qDebug() <<"PLF file empty. Wrote " << total << " bytes.";
    }

    plfMutex->unlock();

}
