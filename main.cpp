#include <Arduino.h>
#include "lab4_inferencing.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// MPU6050
Adafruit_MPU6050 mpu;

// Pins
#define BUTTON_PIN 5      // D3
#define RED_PIN    2      // D0
#define GREEN_PIN  3      // D1
#define BLUE_PIN   4      // D2

// Sampling & buffer
#define SAMPLE_RATE_MS 10
#define CAPTURE_DURATION_MS 2000
#define FEATURE_SIZE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE

bool capturing = false;
unsigned long last_sample_time = 0;
unsigned long capture_start_time = 0;
int sample_count = 0;
float features[FEATURE_SIZE];

void print_inference_result(ei_impulse_result_t result);
void run_inference();
void capture_accelerometer_data();

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

void setup() {
    Serial.begin(115200);
    delay(3000);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);

    Wire.begin(6, 7); // SDA, SCL

    Serial.println("Initializing MPU6050...");
    if (!mpu.begin()) {
        Serial.println("ERROR: MPU6050 not found.");
    } else {
        mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        Serial.println("MPU6050 ready.");
    }

    Serial.println("Press button to start gesture capture.");
}

void capture_accelerometer_data() {
    if (millis() - last_sample_time >= SAMPLE_RATE_MS) {
        last_sample_time = millis();

        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        if (sample_count < FEATURE_SIZE / 3) {
            int idx = sample_count * 3;
            features[idx] = a.acceleration.x;
            features[idx + 1] = a.acceleration.y;
            features[idx + 2] = a.acceleration.z;
            sample_count++;
        }

        if (millis() - capture_start_time >= CAPTURE_DURATION_MS) {
            capturing = false;
            Serial.println("Capture complete.");
            run_inference();
        }
    }
}

void run_inference() {
    if (sample_count * 3 < FEATURE_SIZE) {
        Serial.println("ERROR: Not enough data.");
        return;
    }

    ei_impulse_result_t result = { 0 };
    signal_t features_signal;
    features_signal.total_length = FEATURE_SIZE;
    features_signal.get_data = &raw_feature_get_data;

    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);
    if (res != EI_IMPULSE_OK) {
        Serial.print("Classifier error: ");
        Serial.println(res);
        return;
    }

    print_inference_result(result);
}

void set_led_color(bool r, bool g, bool b) {
    digitalWrite(RED_PIN, r ? HIGH : LOW);
    digitalWrite(GREEN_PIN, g ? HIGH : LOW);
    digitalWrite(BLUE_PIN, b ? HIGH : LOW);
}

void print_inference_result(ei_impulse_result_t result) {
    float max_value = 0;
    int max_index = -1;

    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (result.classification[i].value > max_value) {
            max_value = result.classification[i].value;
            max_index = i;
        }
    }

    if (max_index != -1) {
        const char* label = ei_classifier_inferencing_categories[max_index];
        Serial.print("Prediction: ");
        Serial.print(label);
        Serial.print(" (");
        Serial.print(max_value * 100);
        Serial.println("%)");

        // LED colors for gestures
        if (strcmp(label, "O") == 0) {
            set_led_color(true, false, false);  // Red
        } else if (strcmp(label, "Z") == 0) {
            set_led_color(false, true, false);  // Green
        } else if (strcmp(label, "V") == 0) {
            set_led_color(false, false, true);  // Blue
        } else {
            set_led_color(false, false, false); // Unknown
        }
    }
}

void loop() {
    // Check button press (active low)
    if (!capturing && digitalRead(BUTTON_PIN) == LOW) {
        Serial.println("Button pressed. Starting gesture capture...");
        sample_count = 0;
        capturing = true;
        capture_start_time = millis();
        last_sample_time = millis();
        delay(200); // debounce
    }

    if (capturing) {
        capture_accelerometer_data();
    }
}
