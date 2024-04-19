#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H


//--------------------------  Global Variables  ----------------------------
/* Definitions of variables that apply across the whole application. */
//--------------------------------------------------------------------------


//Other includes
#include <cstring>



// global config for global adhoc fixes and development
// needs to be moved to CMakeList.txt if the become permant configuration 

//In Prototype mode, Groups and Neurons shall follow the same global NID as CARLsim
//#define LN_FIX_PROTOTYPE_IDS

//Redefine the Position key to generate conistent ids intering x->y->z (x as inner loop)
//#define LN_FIX_POSITION_KEY



/*! The maximum length of a name in the database.*/
#define MAX_DATABASE_NAME_LENGTH 250


/*! The maximum length of a description in the database.*/
#define MAX_DATABASE_DESCRIPTION_LENGTH 250


/*! File extension for saving archive file of databases.*/
#define NEURON_APPLICATION_FILE_EXTENSION ".sql.tar.gz"

//Starting and invalid database IDs
/*! First valid automatically generated network ID in NeuralNetwork database */
#define START_NEURALNETWORK_ID 1

/*! A neural network ID that is not valid */
#define INVALID_NETWORK_ID 0

/*! First valid automatically generated neuron group ID in NeuralNetwork database */
#define START_NEURONGROUP_ID 1

/*! First valid automatically generated neuron ID in NeuralNetwork database */
#define START_NEURON_ID 1

/*! Last valid automatically generated neuron ID in NeuralNetwork database.
	Neuron IDs greater than this will be declared invalid */
#define LAST_NEURON_ID 0x7ffffffe

/*! First valid automatically generated ID in NeuralNetwork database */
#define START_CONNECTION_ID 1

/*! Last valid automatically generated connection ID in NeuralNetwork database.
	connection IDs greater than this will be declared invalid */
#define LAST_CONNECTION_ID 0x6ffffffe

/*! First valid automatically generated ID in NeuralNetwork database */
#define START_CONNECTIONGROUP_ID 1

/*! First valid automatically generated ID in Archives database */
#define START_ARCHIVE_ID 1

/*! First valid automatically generated ID in Archives database */
#define START_ANALYSIS_ID 1

/*! First valid automatically generated ID for a weightless neuron pattern */
#define START_WEIGHTLESS_TRAINING_PATTERN_ID 1


//Connection mode flags
/*! Show connections to/from a single neuron */
#define CONNECTION_MODE_ENABLED 1

/*! Show connections between two neurons */
#define SHOW_BETWEEN_CONNECTIONS 4

/*! Show positive connections */
#define SHOW_POSITIVE_CONNECTIONS 8

/*! Show negative connections */
#define SHOW_NEGATIVE_CONNECTIONS 16

/*! Show all connections from a single neuron */
#define SHOW_FROM_CONNECTIONS 32

/*! Show all connections to a single neuron */
#define SHOW_TO_CONNECTIONS 64


//Weight render flags
/*! Render weights as polygons */
#define WEIGHT_RENDER_ENABLED 1

/*! Render temp weights */
#define RENDER_TEMP_WEIGHTS 2

/*! Render current weights */
#define RENDER_CURRENT_WEIGHTS 4

/*! Render weights style synapse */
#define RENDER_WEIGHT_SYNAPSE 8

#define RENDER_WEIGHT_MIDDLE 16

#define RENDER_WEIGHT_EVEN 32


//Delay render flags
/*! Render delays as polygons */
#define DELAY_RENDER_ENABLED 1

/*! Render temp delays */
#define RENDER_TEMP_DELAYS 2

/*! Render current delays */
#define RENDER_CURRENT_DELAYS 4


///*! Render delays as myelin sheath thickness */
//#define RENDER_DELAYS 8
//// https://medlineplus.gov/ency/article/002261.htm#:~:text=Myelin%20is%20an%20insulating%20layer,damaged%2C%20these%20impulses%20slow%20down.
//// https://medlineplus.gov/ency/imagepages/9682.htm
//// data:image/jpeg;base64,/9j/4AAQSkZJRgABAQAAAQABAAD/2wCEAAkGBwgHBgkIBwgKCgkLDRYPDQwMDRsUFRAWIB0iIiAdHx8kKDQsJCYxJx8fLT0tMTU3Ojo6Iys/RD84QzQ5OjcBCgoKDQwNGg8PGjclHyU3Nzc3Nzc3Nzc3Nzc3Nzc3Nzc3Nzc3Nzc3Nzc3Nzc3Nzc3Nzc3Nzc3Nzc3Nzc3Nzc3N//AABEIAKAAhAMBIgACEQEDEQH/xAAbAAEAAwEBAQEAAAAAAAAAAAAAAQMEBQIGB//EAEAQAAEDAgMEBQkFBgcAAAAAAAEAAgMEERIhMQUTQVEzYXGB0RQiMlJzoaKxsgZCVJGSFSNDYmPwJHSUwcLS8f/EABkBAQEBAQEBAAAAAAAAAAAAAAABAgMEBf/EACMRAQEAAgEEAQUBAAAAAAAAAAABAhESAzFBYSETIjJCUQT/2gAMAwEAAhEDEQA/AP1/adXV0rovJKR1Q0hxkDdRa1rduYVJ2rVhuIbLnIa0FwvmXchln26Zrr27VBHJBVSSvlhD5ot083GC9+JV11nkk8x+VpGDEBzty6l6fPhazCMTn+iApuC9LquMOAu8i55KqpmMTH8Da4KW6mxoBB0UrLE6YxDBGxotljdn7kAq8Vy6HssQkGpFn/xXOD3ry/yvdvwmLFgOG19eGqo1IuO1+3AMJZSZOtckl2HPM8L6X01NtBfTQO2kZpBXCDd28wxgg348UG9ERAREQEREEHMWVLnPj1GNvVqPFXlVPjxaveOw2UvoQ5kdQwHUWuHA2IWRzX0skJcccTLtBAzF1dT2hqJIbmxGMXP5pVvDnNhbm5zgT1ALln+PLy3j309Ctpz/ABWjtyWWtmZUNZHFdxLx52gWDaG1XxTPFPC17GZFxNs1ML2TTU0rnSbuQ3J3rha4y45LzX/VyymDt9G4zk74yCricXve7gHYQOxeBSRW9Of/AFD/ABWdsMMRmEj5gGHFffv0Pevba87oIcliiip3txGaQX5VT/8AsvYpYXC7Xz2/zEnirLtGm6A3XNqNkRT1EkwmnjMtsW7fhBsLBaqKlFK14bJI/G4u883t1DqyVGpERAREQEREEHRUSukIOGzGjVxN7LQVW9oeQDoOHNSzcGOKkMjWySSyh9yQcWgVbWbiRzQ8uxtOZOYPWug97GNu9wa3mSubVyiSZopW7w2JOFebrY4zD47uuFu/T5moNRHI+lMUrnOecGFhNxzuuvBG+GGKnwtldhsGAHPrV7o6iEY54LM0uxwJHLLir9mFnlb3kEOewBoPC2oXh6XStzkzmnq6vV3j8LYGbRjYB+5I5PJuO9eIoqiSqe6bdYhngN7dq6eSqlY4PbLGLubkRzC+nwk16eLlt5wz39CD8z4KuqNcKR/kzYN/lgDr4TnnfuWlkrHZB2fI5Fe10Zcl022PuUcGR4ya65/LLr7lvo3zvY41MTY34iAGuvdvDvV+SKiUUKUBERAREQQVU5sv3ZGgdbVcVClg57yxsos108xvhc4ZDmtNLBumHEbvcbuKjAG1EdhlhI+S0LOOPztbfDNURP3jJmeeWfwzp2jrVFRGyaITw3xD7w1y59hXQWa26q8h5kw0/mA/3HyW9S90l12c3Y9c+sfPTy1EraiFxBFmi452supuH/iZvh8Fy5qaCh2myscWtF7OcG2NjcAE8QF21rPW9wUbh/4mb4fBNw/8VN8PgrybapdZFG4f+Km+HwTcP/FTfD4K+6IMk8ckTWyColJD2Cxw5guA5LYs9Z0A9oz6gtCAiIgIiIBUKSoQRYXB4hSiICoqxZkbhq2Vlu8gH3Eq9UVnRN9rH9QQcj7Uxyup27kZm1/5gDp7/curG2qMbCZYwbC4MRv9S532mrDSUrHN1LgGjr4FdeEl0LHO9ItBK3d8YjJVMriIjTzR4muJfdtg4cuJ96ziPbTS1u+o3CzbktdfUX0y58terPqmylYVzMG1twAJaTek5ksNrWHDtxe5bKNs7KZjaqQSSj0nNFgVeiCis6Ae0Z9QWhZ63oB7Rn1BI6+jke+OOqhc9nptDxduZGfe0juKDQizitpTDvhURbq18eMWtrqpfWU0ZaJKiJpc7C3E8C55BBeiIgFQpKhAREQYq6qqad8Ygo31DSDjwEAg5W1714jqZakR7+mdB+9bYON72BN/cugqKjpKc8BL/wAXBBz9txwyYBO60ej7i9hf/wBW8UoAAEswA/qFcT7U78yQiIEswkyWPDNfRNNwD1LeU1jKipkAa4O3sp6i+4XLGwGNY6NldWNjde7RJrlht2AWXZJATEFhVNHT+TRFgllkBcXAyuuR1divUYglwgpregHtGfUFkl2BsuaZ0slHGXvfjcfWdrc9a11nQD2jPrC0IOd+wtmbsx+Rx4CQS3OxsCB7ifzXhv2e2Qxxc2hjBJvqebjz5uce9dREAZIiIBUKSoQEREBUVnQg+rIw/EFevEzBLC+N17OaQbIMO2y1lLjePQBdi5EDL++pNmbusoKeoJlDpGXcGzvsHcbZ87r3URftDZpjdbE9tnDSx0I+YWLYcT6SF1FFMy8biQyUEut3Hndb+3h7Pluq9mwVUG5kdNhve+9cSDYjIntVEex4o5JHipqi57Cw3kyAN/liNlttVevD+g+KWqvXh/QfFYGB+wqUhmGWoY5jC0PZJYkFxce+51VjNjwsqBMJ6m7SCG704cr5dmZWu1V68H6T4paq9eH9B8UCs6Ae0Z9QWhZHxVEgAfJEG4mk2Yb5EHn1LWgIiICIiAVCkqEBERAQ6IiDMP3NSQfQmNweT+XePkVwtqeU0O2IKmFpfE1hJAtkOI77+4L6OaNsrCx2h4jUHmOtZJJWYDDVAGUZAAZv62j+7LWOXEbWm41Urn01Q6DDFUtw+qRmAFfJWRMHmnGeTc1xnUx/q3GrJySAwOLS46jVR5P/AFpv1qpkcssm9dIGZZNaAbLQxrwfOfiHWFqXYonjdEwPbNLfGwZu5uAK1rPWdAPaM+oLQtIIiICIiAVCiRwYwvcQGtFyTwCppaynq2udTTNkDbYsJ0uLj3FBeiIgIiICgga2zUqqaZsQ845nQcSVLdDNUNE9VHFnZoJdZeYMEE7qdwBadCbX7FZRY95NvRZ5IJHIKmZgeXSuF2ibCRyboVy4/tO7pvw27mLKzGg3vcCysBB0IKyvpcm4XPeBqxzjYhWRxQuaMMQaAcvNsQuk3tgregHtGfUFjdt3Z7JXxPqML2PLC3CSTawysOsLZWdAPaM+oK7AMtMupaRzn7c2fHLunzkP80gCNxuHC4tYKGbe2e8sa2oJc9wa0YHC5Ivy5LpYBe+X5JgbyF+dkHoaIiIMO03zMhOFjDEWlsmJxBF7DK3aVz9kSTAltOY5Q7AXufOXEAADLzRwC7M8ZkaAJHMzvcKuCndE8uMz3ggixQaEREBERAWenAkc+Z2ZJIaepXPbjaW3IvyUtaGgAaBTyK2sIne4jKwAPNGRWa9r7ODiT3FWomhjjbPCcGMOH3cfLlcLRHJiOEgtcBm0qZY9422JzDqHNtcfmqfJXk3NVMSNDhjy+FSY6W3b1WdAPaM+oLQsxpXOtjqpnNDg7CQyxsb8G9S0rSCIiAiIg//Z
//// https://en.wikipedia.org/wiki/Myelin
//// https://en.wikipedia.org/wiki/Saltatory_conduction
//

//Heat colour map colours
/*! Heat map colour 0 */
#define HEAT_COLOR_0 0, 0.5, 0

/*! Heat map colour 1 */
#define HEAT_COLOR_1 0, 0, 1.0

/*! Heat map colour 2 */
#define HEAT_COLOR_2 0.125, 0, 0.875

/*! Heat map colour 3 */
#define HEAT_COLOR_3 0.25, 0, 0.75

/*! Heat map colour 4 */
#define HEAT_COLOR_4 0.375, 0, 0.625

/*! Heat map colour 5 */
#define HEAT_COLOR_5 0.625, 0, 0.375

/*! Heat map colour 6 */
#define HEAT_COLOR_6 0.75, 0, 0.25

/*! Heat map colour 7 */
#define HEAT_COLOR_7 0.875, 0, 0.125

/*! Heat map colour 8 */
#define HEAT_COLOR_8 1.0, 0, 0

/*! Heat map colour 9 */
#define HEAT_COLOR_9 1.0, 0.5, 0

/*! Heat map colour 10 */
#define HEAT_COLOR_10 1.0, 1.0, 0


/*! Default value associated with heat colour 10.
	Heat colour 0 is always associated with 0. */
#define DEFAULT_MAX_HEAT_COLOR_VALUE 10.0


/*! A state based phi analysis */
#define STATE_BASED_PHI_ANALYSIS "statebasedphianalysis"


/*! A liveliness analysis */
#define LIVELINESS_ANALYSIS "livelinessanalysis"


#endif //GLOBALVARIABLES_H