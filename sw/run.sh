#/bin/bash

NUM_CORES=$(nproc) # On Linux

builddir="build/"
dumpdir="temp"

tshark_testfile=outbytes.txt
tshark_outputfile=outbytes.pcap
log_file=log.txt

echoreq_outfile=echoreq.txt
echoreq_pcap=echoreq.pcap
echoresp_outfile=echoresp.txt
echoresp_outfile=echoresp.pcap

tshark_inputfile=inbytes.pcap

source .venv/bin/activate
case $1 in
-t)
    make clean

    ret=$(bear -- make -j$NUM_CORES MODE=sw -s)
    if [[ $? -ne 0 ]]; then
        exit
    fi

    mv compile_commands.json build/
    #make dump
    echo -e "Running Sim\n\n"

    echo "========================================"
    echo "⚙️ 1. Running test executable which sends dummy data"
    echo "========================================"
    build/sw/exe.elf

    mkdir -p ${dumpdir}/

    echo "========================================"
    echo "📦 2. Converting raw text hex dump to PCAP using text2pcap"
    echo "========================================"
    # Uncomment the line below if raw file needs Ethernet header
    # text2pcap -e 0x0800 ${inputfile} ${outputfile}
    text2pcap ${dumpdir}/${tshark_testfile} ${dumpdir}/${tshark_outputfile}

    echo "========================================"
    echo "🔎 3. Inspecting converted PCAP with tshark"
    echo "========================================"
    tshark -r ${dumpdir}/${tshark_outputfile} -o ip.check_checksum:TRUE -V

    echo "✅ Done."

    ;;
-r)
    make clean

    ret=$(bear -- make -j$NUM_CORES MODE=sw -s)
    if [[ $? -ne 0 ]]; then
        exit
    fi

    mv compile_commands.json build/

    echo "========================================"
    echo "🐍 1. Running Python script to generate test packets"
    echo "========================================"
    python3 testingProtocol/dummy.py random_frame

    echo "========================================"
    echo "🔍 2. Running tshark on original PCAP to inspect IP checksum"
    echo "========================================"

    echo "========================================"
    echo "⚙️ 3. Running test executable to observe parse"
    echo "========================================"
    build/sw/exe.elf

    echo -e "\n\n\n"

    ;;

-p)
    echo "========================================"
    echo "🐍 1. Running Python script to gen echo req"
    echo "========================================"

    mkdir -p ${dumpdir}/

    python3 testingProtocol/echoreq.py

    text2pcap ${dumpdir}/${echoreq_outfile} ${dumpdir}/${echoreq_pcap}
    tshark -r ${dumpdir}/${echoreq_pcap} -o ip.check_checksum:TRUE -V

    make clean

    ret=$(bear -- make -j$NUM_CORES MODE=sw -s EXTRA_CFLAGS="-DECHOTEST")
    if [[ $? -ne 0 ]]; then
        exit
    fi

    mv compile_commands.json ${builddir}
    #make dump
    echo -e "Running Sim\n\n"
    build/sw/exe.elf

    touch ${dumpdir}/echoresp.txt
    text2pcap ${dumpdir}/echoresp.txt ${dumpdir}/echoresp.pcap
    tshark -r ${dumpdir}/echoresp.pcap -o ip.check_checksum:TRUE -V
    ;;

-d)
    make clean
    ret=$(bear -- make -j$NUM_CORES MODE=sw -s EXTRA_CFLAGS=-DDHCPTEST)
    if [[ $? -ne 0 ]]; then
        exit
    fi

    mv compile_commands.json build/
    #make dump
    echo -e "Used $NUM_CORES for buidling\n"
    echo -e "Running Sim\n\n"

    mkdir -p ${dumpdir}

    echo "========================================"
    echo "⚙️ 1. Running test executable which sends dummy data"
    echo "========================================"
    build/sw/exe.elf

    echo "========================================"
    echo "📦 2. Converting raw text hex dump to PCAP using text2pcap"
    echo "========================================"
    # Uncomment the line below if raw file needs Ethernet header
    # text2pcap -e 0x0800 ${inputfile} ${outputfile}
    text2pcap temp/dhcp_disc.txt temp/dhcp_disc.pcap

    echo "========================================"
    echo "🔎 3. Inspecting converted PCAP with tshark"
    echo "========================================"
    tshark -r temp/dhcp_disc.pcap -o ip.check_checksum:TRUE -V

    echo "========================================"
    echo "🔎 4. Generating offer response..."
    echo "========================================"

    python3 testingProtocol/dummy.py dhcp_offer

    echo "========================================"
    echo "🔎 5. Inspecting offer response with tshark"
    echo "========================================"
    tshark -r temp/dhcp_offer.pcap -o ip.check_checksum:TRUE -V

    echo "✅ Done."
    ;;
-dd)
    make clean
    ret=$(bear -- make -j$NUM_CORES MODE=sw EXTRA_CFLAGS=-DDHCPTEST -s)
    if [[ $? -ne 0 ]]; then
        exit
    fi

    mv compile_commands.json build/
    #make dump
    echo -e "Used $NUM_CORES for buidling\n"
    echo -e "Running Sim\n\n"

    mkdir -p ${dumpdir}

    echo "========================================"
    echo "⚙️ 1. Run python script "
    echo "========================================"

    python3 testingProtocol/dummy.py dhcp_offer

    echo "========================================"
    echo "🔎 2. Inspecting offer response with tshark"
    echo "========================================"
    tshark -r temp/dhcp_offer.pcap -o ip.check_checksum:TRUE -V

    echo "========================================"
    echo "📦 3. Run rx"
    echo "========================================"
    build/sw/exe.elf

    echo "========================================"
    echo "📦 4. Converting raw text hex dump to PCAP using text2pcap"
    echo "========================================"
    # Uncomment the line below if raw file needs Ethernet header
    # text2pcap -e 0x0800 ${inputfile} ${outputfile}
    text2pcap temp/dhcp_req.txt temp/dhcp_req.pcap

    echo "========================================"
    echo "🔎 5. Inspecting converted PCAP with tshark"
    echo "========================================"
    tshark -r temp/dhcp_req.pcap -o ip.check_checksum:TRUE -V

    echo "========================================"
    echo "🔎 6. Generating ack response..."
    echo "========================================"
    python3 testingProtocol/dummy.py dhcp_ack

    echo "========================================"
    echo "🔎 7. Inspecting offer response with tshark"
    echo "========================================"
    tshark -r temp/dhcp_ack.pcap -o ip.check_checksum:TRUE -V

    echo "✅ Done."

    ;;

-a)
    make clean
    ret=$(bear -- make -j$NUM_CORES MODE=sw EXTRA_CFLAGS=-DDHCPTEST -s)
    if [[ $? -ne 0 ]]; then
        exit
    fi

    mv compile_commands.json build/
    #make dump
    echo -e "Used $NUM_CORES for buidling\n"
    echo -e "Running Sim\n\n"

    mkdir -p ${dumpdir}

    echo "========================================"
    echo "⚙️ 1. Run python script "
    echo "========================================"

    python3 testingProtocol/dummy.py dhcp_ack

    echo "========================================"
    echo "🔎 2. Inspecting offer response with tshark"
    echo "========================================"
    tshark -r temp/dhcp_ack.pcap -o ip.check_checksum:TRUE -V

    echo "========================================"
    echo "📦 3. Run rx"
    echo "========================================"
    build/sw/exe.elf
    
    echo "✅ Done."

    ;;


-b)
    make clean

    ret=$(bear -- make -j$NUM_CORES MODE=hw -s)
    if [[ $? -ne 0 ]]; then
        exit
    fi

    mv compile_commands.json build/

    make flash MODE=hw -s
    #make dump
    echo -e "Used $NUM_CORES for buidling\n"
    echo -e "Flashing to Target..."
    ;;
*)
    echo -e "try again shitter\n"
    ;;
esac
