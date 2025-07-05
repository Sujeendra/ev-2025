# ev-2025

A generalized framework for J1939 CAN communication, validated via HIL testing and real-world deployment on an electrified compact track loader. Includes a Qt-based dashboard for control and monitoring, addressing integration challenges for off-road/heavy-duty EVs. Promotes standardized J1939 implementation.

Please follow the [link](https://ev-2025.web.app/) for more information on how to use this repository.

## Citation

If you use this framework in your work, please cite:

```bibtex
@inproceedings{ramesh2025communication,
  title     = {Communication Framework for Electrified Off-Road Vehicles: A Case Study on the HHEA Compact Track Loader},
  author    = {Sujeendra Ramesh and Perry Y. Li},
  booktitle = {Proceedings of the IEEE International Conference on Vehicular Electronics and Safety (ICVES)},
  year      = {2025},
  organization = {University of Minnesota, Twin Cities},
  address   = {Minneapolis, USA},
  note      = {Department of Robotics and Mechanical Engineering, University of Minnesota},
  email     = {rames154@umn.edu, lixxx099@umn.edu}
}
```
## HHEA: Hybrid Hydraulic Electric Architecture


Hello,

I am **Sujeendra Ramesh**, and this innovative software framework was developed as part of my master's thesis at the **University of Minnesota, Twin Cities**, in the field of robotics in 2025.

This project is a part of [**Dr. Li's Lab**](https://sites.google.com/umn.edu/perry-li/research/miml?authuser=0), and the project name is **HHEA (Hybrid Hydraulic Electric Architecture)**. I would like to extend my gratitude to my entire team, including:

- Dr. Li
- Jim
- Ravi
- Jackson
- Jacob
- Mahmud
- Zihao
- Dr. Severson

* This work is supported by the **U.S. Department of Energy Office of Energy Efficiency and Renewable Energy’s (EERE) Vehicle Technologies Office (VTO)** under grant number **DE-EE0009875**. Their support has been instrumental in enabling the research and development presented in this thesis.

### Background on HHEA

The **Hybrid Hydraulic-Electric Architecture (HHEA)** is an advanced powertrain designed for off-road electrified vehicles. The prototype developed for this thesis is an **electrified Compact Track Loader**, which combines the **power density of hydraulics** with the **efficiency and controllability of electric actuation**.

Central to HHEA is a **set of discrete hydraulic pressure rails**, each maintained at different pressure levels. Actuators and motors are connected to valve manifolds that dynamically pair these rails using solenoid valves controlled by the **Vehicle Control Unit (VCU)**. This results in **16 possible pressure pairings**, enabling fine control over actuator force and motor torque.

A **Digital Displacement Pump (DDP)**, driven by a **Danfoss Editron motor and inverter**, supplies and stabilizes the pressure rails based on system demand. Rather than throttling, this centralized flow control enhances energy efficiency.

The system also integrates **four smaller motor-generator (M/G) units** — **Parker GVM210 motors** with **Sevcon inverters** — mounted on hydraulic pump/motor assemblies. These motors serve:

- **Left and right propulsion** (coupled with hydraulic drive motors)
- **Lift and tilt functions** (paired with hydraulic actuators)

The M/Gs, sized for ~15% of peak power, handle **fine torque modulation**, **pressure boosting**, and **energy recovery**, effectively "bridging the gap" between discrete hydraulic steps and ensuring smooth, responsive control.

This hybrid approach achieves high efficiency, dynamic performance, and robust control for complex vehicle subsystems.

---

If you have any questions, please feel free to contact me at [rames154@umn.edu](mailto:rames154@umn.edu).

---
