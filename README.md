# Translator of printed Spanish text into Braille System
## (Traductor de textos en Español al Sistema Braille)

## How to cite
Please cite as:

E. I. Valle Salgado et al., "Translator of printed Spanish texts into Braille system," IX Congreso Iberoamericano de Tecnología de Apoyo a la Discapacidad, p.544, November 2017. [Online]. https://www.escuelaing.edu.co/escuela/iberdiscap2017/pdf/Memorias-Iberdiscap2017-ISSN.pdf [Accessed: Oct. 7, 2020].

## 1. Abstract
In this research is presented the proposal for the design, the construction and the implementation of a translator of printed Spanish texts into Braille system, which aims to facilitate the access to information from documents that are not commonly available to people with visual disabilities. The translator is comprised of a mechanical system with linear actuators, which generates the Braille characters for reading, software designed for converting the acquired information from a text to recognizable symbols by this mechanism. It is possible to save, close and erase acquired data via camera to a text file. For improving the experience between the user and this device, a “text to speech” software is included to read instructions, menu and documents. An easy interface allows users to navigate quickly throw the main menu in order to manage files, get information about battery, storage, volume, etc. This device contains more features like translating Portable Document Format (PDF) and Microsoft word documents into Braille system.

## 2. Platform description
This description provides information about the inputs and outputs of information, energy and of each one of the subsystems in order to carry out the functions that meet the translator's objective.

### 2.1. Interface
Contains a switch and four buttons to perform the following functions:
  a) On / Off
  b) Menu
  c) Ok
  d) Next
  e) Previous

The main menu allows users to navigate easily through reading and writing options and indications by means of a text to speech API and the relief matrix respectively. An audio output is included to plug headphones or speakers in this device.

### 2.2. Central Process Unit
This unit transforms the previous acquired ASCII characters from an image into recognizable signals so that the Braille matrix generates the relief of the letters, controls the actuators and receives user’s commands through the interface.
In order to perform the programming and execution of all the sections involved in the system, it is necessary to contemplate an environment that provides the functionality of an operating system in a heterogeneous cluster. Thus, the platform ROS (Robot Operating System) provides standard services of an operating system such as hardware abstraction, low-level device control, implementation of commonly used functionality, message passing between processes and maintenance of packages. It is based on an architecture of graphs where the processing takes place in the nodes that can receive, send and multiplex messages of sensors, control, states, schedules and actuators, among others (Morgan Quigley, 2009). The library is based on UNIX system, resulting convenient because this project uses Debian 9 Jessie (a distribution).

### 2.3. Computer Vision Algorithms
We propose an algorithm to extract the text from a paper sheet using a camera to get an image and then process it. The algorithm performs seven steps: acquires an image, detects the border edge, detects Hough transform line, extracts area of interest, crops and rotates the image, gets the Wolf-Jolion binarization and finally transforms the binarized image into raw text. An OCR algorithm (Tesseract OCR) identifies text contained in an image and transforms it into an ASCII code. Figure 3 show the proposed algorithm.

### 2.4. Acquisition System
This section is responsible of image acquisition containing text, it is integrated in hardware by a Raspberry Pi Camera V2.1, and a 16 GB SD memory card that stores information for further processing and character recognition.
### 2.5. Electrical Supply
It includes a power module to supply actuators and another one for CPU and peripherals. Battery charger is included.

### 2.6. Relief Matrix
Set of 10 matrices of 2x3 points, controlled by linear actuators that react to electrical signals, has a control for the position of the actuators generated by the CPU. The positions of the matrix points, as well as their elevations, are based on the standards of the Center of Braille Innovation of the National Braille Technology (Runyan, 2009).
### 2.7. Structure
Structure with asymmetric shape that allows to manipulate the device easily. Its design contains all the components of the device. The design of the structure has enough space to place each part that integrates the translator. Its dimensions are 20cm x 12.5cm x 4cm. The camera is located at the bottom, and rubber supports at the four corners. Figure 4 shows the purposed system design.

### Sources: 
Morgan Quigley, K. C. (2009). ROS: an open-source Robot Operating System. ICRA workshop on open source software, 5.
Runyan, N. H. (2009). EAP Braille Display Needs and Requirements. Boston: National Braille Press (NBP).
