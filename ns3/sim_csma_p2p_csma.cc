// TODO: add copyright
// TODO: add simulation scenario discription
// client <-CSMA-> router <-P2P-> router <-CSMA-> server

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/tap-bridge-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("sim_csma_p2p_csma");

int main(int argc, char *argv[]) {
    std::string animFile = "scratch/sim_csma_p2p_csma/logs/sim_csma_p2p_csma.xml";
    std::string errorModelType = "ns3::BurstErrorModel";
    std::string dataRate = "1Gbps";
    double er = 0.0;
    long owd = 0;
    bool logNetanim = false;
    bool logPcap = false;
    long minBurstSize = 1;
    long maxBurstSize = 5;

    #if 1
        LogComponentEnable("sim_csma_p2p_csma", LOG_LEVEL_ALL);
    #endif

    CommandLine cmd(__FILE__);
    cmd.AddValue("dataRate", "P2P channel data rate", dataRate);
    cmd.AddValue("errorModelType", "TypeId of the error model", errorModelType);
    cmd.AddValue("er", "Error Rate for error model", er);
    cmd.AddValue("owd", "One-Way propagation Delay of P2P channel in milliseconds", owd);
    cmd.AddValue("logNetanim", "Turn on NetAnim log components", logNetanim);
    cmd.AddValue("logPcap", "Turn on pcap log components", logPcap);
    cmd.AddValue("minBurstSize", "Minimum burst size", minBurstSize);
    cmd.AddValue("maxBurstSize", "Maximum burst size", maxBurstSize);
    cmd.Parse(argc, argv);

    if (minBurstSize > maxBurstSize) {
        NS_LOG_INFO("minBurstSize should be lower than maxBurstSize");
        NS_LOG_INFO("Exit.");
        return 0;
    }

    std::string uniformRandomVarDefinition = "ns3::UniformRandomVariable[Min=" + std::to_string(minBurstSize) +
        "|Max=" + std::to_string(maxBurstSize) + "]";
    NS_LOG_INFO(uniformRandomVarDefinition);
    NS_LOG_INFO("error IS");
    NS_LOG_INFO(er);

    // Comment the following line (first line only) to enable Python visualization
    // Note: not recomended for real tests
    GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));
    GlobalValue::Bind("ChecksumEnabled", BooleanValue(true));

    // Configuration for BurstErrorModel which will be used for P2P channel
    Config::SetDefault("ns3::BurstErrorModel::ErrorRate", DoubleValue(er));
    //Config::SetDefault("ns3::BurstErrorModel::BurstSize", StringValue(uniformRandomVarDefinition));

    // Configuration for RateErrorModel which will be used for P2P channel
    Config::SetDefault("ns3::RateErrorModel::ErrorRate", DoubleValue(er));
    Config::SetDefault("ns3::RateErrorModel::ErrorUnit", StringValue("ERROR_UNIT_PACKET"));

    ObjectFactory factory;
    factory.SetTypeId(errorModelType);
    Ptr<ErrorModel> em = factory.Create<ErrorModel>();

    NS_LOG_INFO("Create client node");
    NodeContainer client;
    client.Create(1);

    NS_LOG_INFO("Create router nodes");
    NodeContainer routers;
    routers.Create(2);

    NS_LOG_INFO("Create server node");
    NodeContainer server;
    server.Create(1);

    // Internet stack configuration for router nodes
    InternetStackHelper stack;
    stack.Install(routers);

    NS_LOG_INFO("Create topology");
    // CSMA related configuration
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("1Gbps"));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(0.0)));

    // P2P related configuration
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", DataRateValue(DataRate(dataRate)));
    p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(owd)));

    // Create links
    NetDeviceContainer csmaCR;
    NetDeviceContainer p2pRR;
    NetDeviceContainer csmaRS;

    csmaCR = csma.Install(NodeContainer(client, routers.Get(0)));
    p2pRR = p2p.Install(routers);
    csmaRS = csma.Install(NodeContainer(routers.Get(1), server));

    NS_LOG_INFO("Assign IP addresses for routers");
    Ipv4AddressHelper ip;
    ip.SetBase("192.168.42.0", "255.255.255.0", "0.0.0.201");
    ip.Assign(csmaCR.Get(1));
    ip.SetBase("192.168.43.0", "255.255.255.0", "0.0.0.201");
    ip.Assign(p2pRR);
    ip.SetBase("192.168.44.0", "255.255.255.0", "0.0.0.201");
    ip.Assign(csmaRS.Get(0));

    NS_LOG_INFO("Enable global routing");
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    NS_LOG_INFO("Assign error model to P2P channel");
    p2pRR.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(em));
    p2pRR.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));

    NS_LOG_INFO("Create tap bridge for client");
    TapBridgeHelper tapCSMABridge;
    tapCSMABridge.SetAttribute("Mode", StringValue("UseBridge"));
    tapCSMABridge.SetAttribute("DeviceName", StringValue("tap_sim1"));
    tapCSMABridge.Install(client.Get(0), csmaCR.Get(0));

    NS_LOG_INFO("Create tap bridge for server");
    tapCSMABridge.SetAttribute("DeviceName", StringValue("tap_sim2"));
    tapCSMABridge.Install(server.Get(0), csmaRS.Get(1));

    // Configure nodes placement and mobility
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    positionAlloc->Add(Vector(0.0, 5.0, 0.0));
    positionAlloc->Add(Vector(5.0, 5.0, 0.0));
    positionAlloc->Add(Vector(10.0, 5.0, 0.0));
    positionAlloc->Add(Vector(15.0, 5.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(client);
    mobility.Install(routers);
    mobility.Install(server);

    // Configure NetAnim tracing
    if (logNetanim) {
        AnimationInterface anim(animFile);

        anim.UpdateNodeSize(0, 1, 1);
        anim.UpdateNodeSize(1, 1, 1);
        anim.UpdateNodeSize(2, 1, 1);
        anim.UpdateNodeSize(3, 1, 1);

        anim.UpdateNodeColor(0, 0, 0, 255);
        anim.UpdateNodeColor(1, 0, 255, 0);
        anim.UpdateNodeColor(2, 0, 255, 0);
        anim.UpdateNodeColor(3, 255, 0, 0);
        
        anim.UpdateNodeDescription(0, "Client");
        anim.UpdateNodeDescription(1, "Router1");
        anim.UpdateNodeDescription(2, "Router2");
        anim.UpdateNodeDescription(3, "Server");
    }

    // Configure pcap tracing
    if (logPcap) {
        csma.EnablePcapAll("dumps/csma-dump");
        p2p.EnablePcapAll("dumps/p2p-dump");
    }

    // Start simulation
    Simulator::Stop(Seconds(6000.0));
    Simulator::Run();

    Simulator::Destroy();
}
