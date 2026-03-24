# Cell-DEVS-Bee-Foraging
A Cadmium simulation model of bee foraging

---

## FILES ORGANIZATION

**Makefile**  
Build and run targets: `make all` (build both executables), `make simulation`, `make tests`, `make clean`.

---

### **main/**  
*Entry point and source for the full nectar model simulation.*

- main.cpp


---

### **model/**  

- /cells/nectar_cell.hpp
- /cells/nectar_state.hpp
- nectar_grid.hpp

---

### **tests/**  
*Unit tests for atomic and coupled models.*

- main.cpp
- test.hpp

### **tests/results/**  
*Output files contianing the results of each atomic and coupled model test(s).*

---

### **config/**  
*Input data to run the model and the tests.*

- initial_scenario.json

---

### **simulation_results/**  
*Created automatically the first time you run the simulation. It stores the outputs from your simulations and tests.*

---

### **docs/**  
*Assignment and model form documentation.*

### **.github/workflows/**  
*CI configuration (e.g. release.yml).*

---

## STEPS

### **0 – Model documentation**  
The **docs/** folder contains the explanation of this model (e.g. assignment and model form docs).

### **1 - Run the simulaton**
```make```
```./nectar```