// COMP10060 PID Controller

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h> // for Sleep()
#endif

float dt = 0.1; // 100ms update time

float accel(float Ku, float L, float currentSpeed, float currentObjective)
{
    //Error calculations
    static float prevError;
    static float integral = 0;
    float error = currentObjective - currentSpeed;
    float derivative = (error - prevError)/dt;


    //PID constant calculations
    float Kp = Ku*.6; 
    float Ti = .5*L;
    float Td = .125*L;


    //Acceleration
    float accel = Kp*(error+(integral/Ti)+(derivative*Td)); // acceleration in (km/h/s)
    
     if(!((accel > 8 && error > 0) || (accel < 0 && error < 0))) //ensures integral error isn't added when the acceleration is greater than 0
    {                               
        integral = integral + error*dt;
    }
    
    if(accel > 8) //caps max acceleration to 8 and ensures it stays positive (no negative acceleration)
    {
        accel = 8;
    }
    else if(accel < 0)
    {
        accel = 0;
    }

    prevError = error;
    return accel;
}

int main()
{
    float currentSpeed = -10;
    float oldSpeed = -10;
    float currentObjective = 20;
    float drag = 0.0002*dt; 
    float resistance = 0.8*dt;
    float Ku = 20;
    float L = .2; 
    float a = 0;
    float input;
    int x;

    printf("Input -1 as a target speed to exit \n");  // "rules"
    printf("Input -2 as a target speed to change the Current Speed \n\n");
   
    while (currentSpeed < 0) {  // get the starting speed, and ensures its always positive
    printf("Input your start speed: ");
    scanf("%f", &currentSpeed);
        if (currentSpeed<0) {
            printf ("Start speed must be greater than or equal to 0\n");
        }
    }

    oldSpeed = currentSpeed;

    while (1) {

    int z=1;

    printf("\nInput your new target speed: "); // to change target speed
    scanf("%f", &input);

    if (input == -1) {  // to end the code
        printf ("End");
        return 0;
    }

    else if (input == -2){ // to change the starting speed
        currentSpeed =-10;

        while (currentSpeed < 0) {  // ensures its always positive

            printf("Input your new speed: ");
            scanf("%f", &currentSpeed);
            
            if (currentSpeed < 0) {
                printf ("new speed must be greater than or equal to 0\n");
            }
        }

        oldSpeed = currentSpeed;
    }

    else if (input > 190) {  // Caps speed at 190. above this speed, air resistance is too great
        printf ("Max speed is 190\nChanging speed to 190\n");
        currentObjective = 190; // if above 190, sets the target speed to 190
    }

    else if (input < 0) {  // ensures its always positive
        printf ("new speed must be greater than or equal to 0\n");
        continue; 
    }

    else {
        currentObjective = input;
    }

    printf (" Time | Speed | Accel \n");
    printf(" 0.00 | %3.2f | %3.2f \n", (currentSpeed), a);

    while (currentSpeed <= currentObjective-.01 || currentSpeed >= currentObjective+.01)// in a real car, it would be a while (1) loop
    { 
        for(int i = 0; i < 10; i++){  // updates every .1 seconds, but prints out every second
        a = accel(Ku, L, currentSpeed, currentObjective);
        currentSpeed = (oldSpeed/(1+drag*oldSpeed))-resistance + a*dt;
        oldSpeed = currentSpeed;
       // Sleep (100); // 100 miliseconds or 0.1 sec
        }

        printf(" %2.2f | %3.2f | %3.2f \n", dt*z*10, (currentSpeed), a);
        z++;
    }

     for(int x = 1; x <= 5; x++) // in a real car, this loop would not exist. its just to test the code the exact number of times needed, 
                                 // instead of testing too much or too little
    {
            for(int i = 0; i < 10; i++)
            {
            a = accel(Ku, L, currentSpeed, currentObjective);
            currentSpeed = (oldSpeed/(1+drag*oldSpeed))-resistance + a*dt;
            oldSpeed = currentSpeed;
           // Sleep (100); // 100 miliseconds or 0.1 sec
            }
            printf(" %2.2f | %3.2f | %3.2f \n", dt*(z+x)*10, (currentSpeed), a);
        }
    }
}
