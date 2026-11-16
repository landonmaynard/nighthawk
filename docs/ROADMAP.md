

# Nighthawk — Development Roadmap

This roadmap outlines the development of Nighthawk, from its initial concept through Level 1 certification and final documentation.

The project is divided into three parallel development tracks that converge during final integration and testing.

```mermaid
flowchart TB
    START(["NIGHTHAWK<br/>Project Development"])

    subgraph MECH["MECHANICAL DESIGN"]
        direction TB
        M1["✓ Initial Concept & Requirements"]
        M2["✓ Preliminary Onshape CAD"]
        M3["✓ Preliminary OpenRocket Simulation"]
        M4["Finalize Mechanical CAD"]
        M5["Finalize Flight Simulations"]
        M6["Acquire Structural Components"]
        M7["Fabricate & Prepare Custom Parts"]
        M8["Assemble Airframe & Structure"]
        M9["Mechanical Assembly & Fit Checks"]

        M1 --> M2 --> M3 --> M4 --> M5
        M5 --> M6 --> M7 --> M8 --> M9
    end

    subgraph ELEC["ELECTRONICS & SOFTWARE"]
        direction TB
        E1["✓ Select Initial Avionics Hardware"]
        E2["✓ Wireless IMU Telemetry Demo"]
        E3["Finalize Telemetry Protocol"]
        E4["Develop Ground Station Software"]
        E5["Design Final Avionics Bay"]
        E6["Design Custom PCB"]
        E7["Assemble Avionics Electronics"]
        E8["Integrate Sensors & Onboard Software"]
        E9["Bench-Test Complete Avionics"]
        E10["Install Avionics Into Rocket"]

        E1 --> E2 --> E3 --> E4 --> E5
        E5 --> E6 --> E7 --> E8 --> E9 --> E10
    end

    subgraph FUND["FUNDING & PREPARATION"]
        direction TB
        F1["✓ Complete Budget & BOM"]
        F2["✓ Publish GitHub Documentation"]
        F3["✓ Prepare Sponsorship & Grant Proposal"]
        F4["✓ Contact Companies & Apply for Grants"]
        F5["✓ Reach $800 Funding Goal"]
        F6["Purchase Remaining Components"]
        F7["NAR / Tripoli Preparation & Paperwork"]

        F1 --> F2 --> F3 --> F4 --> F5 --> F6 --> F7
    end

    START --> M1
    START --> E1
    START --> F1

    M9 --> INTEGRATION
    E10 --> INTEGRATION
    F7 --> INTEGRATION

    subgraph FINAL["FINAL INTEGRATION & LAUNCH"]
        direction TB
        INTEGRATION["Complete Rocket Integration"]
        T1["Full-System Ground Checks"]
        T2["Verify Recovery System Readiness"]
        T3["Final Safety & Launch-Readiness Review"]
        T4["Complete Certification Requirements"]
        LAUNCH(["LEVEL 1 CERTIFICATION LAUNCH"])
        T5["Analyze Flight Data"]
        REPORT(["Final Project Report"])

        INTEGRATION --> T1 --> T2 --> T3 --> T4
        T4 --> LAUNCH --> T5 --> REPORT
    end

    classDef complete fill:#14532d,stroke:#22c55e,color:#ffffff,stroke-width:2px
    classDef mechanical fill:#1e3a8a,stroke:#60a5fa,color:#ffffff
    classDef electronics fill:#115e59,stroke:#2dd4bf,color:#ffffff
    classDef funding fill:#78350f,stroke:#fbbf24,color:#ffffff
    classDef final fill:#374151,stroke:#9ca3af,color:#ffffff
    classDef highlight fill:#581c87,stroke:#c084fc,color:#ffffff,stroke-width:3px
    classDef start fill:#111827,stroke:#9ca3af,color:#ffffff

    class M1,M2,M3,E1,E2,F1,F2,F3,F4,F5 complete
    class M4,M5,M6,M7,M8,M9 mechanical
    class E3,E4,E5,E6,E7,E8,E9,E10 electronics
    class F6,F7 funding
    class INTEGRATION,T1,T2,T3,T4,T5 final
    class LAUNCH,REPORT highlight
    class START start

    style MECH fill:#eff6ff,stroke:#60a5fa,color:#1e3a8a
    style ELEC fill:#f0fdfa,stroke:#2dd4bf,color:#115e59
    style FUND fill:#fffbeb,stroke:#fbbf24,color:#78350f
    style FINAL fill:#f3f4f6,stroke:#9ca3af,color:#111827
```

## Project Targets

| Milestone | Target |
|---|---|
| Preliminary Design & IMU Demo | Completed |
| Funding Goal ($800) | December 2026 |
| Final Assembly & Testing | Spring 2027 |
| Level 1 Certification Launch | Spring 2027 |
| Flight Analysis & Final Report | Following Launch |

**Status:** Development and fundraising in progress.

*Last updated: November 3, 2026*
  
