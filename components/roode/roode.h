#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/application.h"
#include "VL53L1X_ULD.h"
#include <math.h>
#include "configuration.h"
#include "zone.h"

namespace esphome
{
  namespace roode
  {
#define NOBODY 0
#define SOMEONE 1
#define VERSION "v1.4.1-beta"
#define VL53L1X_ULD_I2C_ADDRESS 0x52 // Default address is 0x52
    static const char *const TAG = "Roode";
    static const char *const SETUP = "Setup";
    static const char *const CALIBRATION = "Sensor Calibration";

    /*
    Use the VL53L1X_SetTimingBudget function to set the TB in milliseconds. The TB values available are [15, 20,
    33, 50, 100, 200, 500]. This function must be called after VL53L1X_SetDistanceMode.
    Note: 15 ms only works with Short distance mode. 100 ms is the default value.
    The TB can be adjusted to improve the standard deviation (SD) of the measurement.
    Increasing the TB, decreases the SD but increases the power consumption.
    */

    static int delay_between_measurements = 0;
    static int time_budget_in_ms = 0;

    /*
    Parameters which define the time between two different measurements in various modes (https://www.st.com/resource/en/datasheet/vl53l1x.pdf)
    The timing budget and inter-measurement period should not be called when the sensor is
    ranging. The user has to stop the ranging, change these parameters, and restart ranging
    The minimum inter-measurement period must be longer than the timing budget + 4 ms.
    // Lowest possible is 15ms with the ULD API (https://www.st.com/resource/en/user_manual/um2510-a-guide-to-using-the-vl53l1x-ultra-lite-driver-stmicroelectronics.pdf)
    Valid values: [15,20,33,50,100,200,500]
    */
    static int time_budget_in_ms_short = 15; // max range: 1.3m
    static int time_budget_in_ms_medium = 33;
    static int time_budget_in_ms_medium_long = 50;
    static int time_budget_in_ms_long = 100;
    static int time_budget_in_ms_max = 200; // max range: 4m

    class Roode : public PollingComponent
    {
    public:
      void setup() override;
      void update() override;
      void loop() override;
      void dump_config() override;

      void set_calibration_active(bool val) { calibration_active_ = val; }
      void set_manual_active(bool val) { manual_active_ = val; }
      void set_roi_active(bool val) { roi_active_ = val; }
      void set_roi_calibration(bool val) { roi_calibration_ = val; }
      void set_sensor_offset_calibration(int val) { sensor_offset_calibration_ = val; }
      void set_sensor_xtalk_calibration(int val) { sensor_xtalk_calibration_ = val; }
      void set_timing_budget(int timing_budget) { timing_budget_ = timing_budget; }
      void set_manual_threshold(int val) { manual_threshold_ = val; }
      void set_max_threshold_percentage(int val) { max_threshold_percentage_ = val; }
      void set_min_threshold_percentage(int val) { min_threshold_percentage_ = val; }
      void set_entry_roi_height(int height) { entry_roi_height = height; }
      void set_entry_roi_width(int width) { entry_roi_width = width; }
      void set_exit_roi_height(int height) { exit_roi_height = height; }
      void set_exit_roi_width(int width) { exit_roi_width = width; }
      void set_i2c_address(uint8_t address) { this->address_ = address; }
      void set_invert_direction(bool dir) { invert_direction_ = dir; }
      void set_restore_values(bool val) { restore_values_ = val; }
      void set_advised_sensor_orientation(bool val) { advised_sensor_orientation_ = val; }
      void set_sampling_size(uint8_t size) { DISTANCES_ARRAY_SIZE = size; }
      void set_distance_entry(sensor::Sensor *distance_entry_) { distance_entry = distance_entry_; }
      void set_distance_exit(sensor::Sensor *distance_exit_) { distance_exit = distance_exit_; }
      void set_people_counter(number::Number *counter) { this->people_counter = counter; }
      void set_max_threshold_entry_sensor(sensor::Sensor *max_threshold_entry_sensor_) { max_threshold_entry_sensor = max_threshold_entry_sensor_; }
      void set_max_threshold_exit_sensor(sensor::Sensor *max_threshold_exit_sensor_) { max_threshold_exit_sensor = max_threshold_exit_sensor_; }
      void set_min_threshold_entry_sensor(sensor::Sensor *min_threshold_entry_sensor_) { min_threshold_entry_sensor = min_threshold_entry_sensor_; }
      void set_min_threshold_exit_sensor(sensor::Sensor *min_threshold_exit_sensor_) { min_threshold_exit_sensor = min_threshold_exit_sensor_; }
      void set_entry_roi_height_sensor(sensor::Sensor *roi_height_sensor_) { entry_roi_height_sensor = roi_height_sensor_; }
      void set_entry_roi_width_sensor(sensor::Sensor *roi_width_sensor_) { entry_roi_width_sensor = roi_width_sensor_; }
      void set_exit_roi_height_sensor(sensor::Sensor *roi_height_sensor_) { exit_roi_height_sensor = roi_height_sensor_; }
      void set_exit_roi_width_sensor(sensor::Sensor *roi_width_sensor_) { exit_roi_width_sensor = roi_width_sensor_; }
      void set_sensor_status_sensor(sensor::Sensor *status_sensor_) { status_sensor = status_sensor_; }
      void set_presence_sensor_binary_sensor(binary_sensor::BinarySensor *presence_sensor_) { presence_sensor = presence_sensor_; }
      void set_version_text_sensor(text_sensor::TextSensor *version_sensor_) { version_sensor = version_sensor_; }
      void set_entry_exit_event_text_sensor(text_sensor::TextSensor *entry_exit_event_sensor_) { entry_exit_event_sensor = entry_exit_event_sensor_; }
      void set_sensor_mode(int sensor_mode_) { sensor_mode = sensor_mode_; }
      uint16_t getAlternatingZoneDistances();
      void doPathTracking(Zone *zone);
      void recalibration();
      bool handleSensorStatus();

      uint16_t distance = 0;

      ERangeStatus rangeStatus;
      int entry_roi_width{6};   
      int entry_roi_height{16}; 
      int exit_roi_width{6};    
      int exit_roi_height{16}; 
      uint16_t peopleCounter{0};
      Configuration sensorConfiguration;

    protected:
      VL53L1X_ULD distanceSensor;
      Zone *entry;
      Zone *exit;
      Zone *current_zone;
      sensor::Sensor *distance_entry;
      sensor::Sensor *distance_exit;
      number::Number *people_counter;
      sensor::Sensor *max_threshold_entry_sensor;
      sensor::Sensor *max_threshold_exit_sensor;
      sensor::Sensor *min_threshold_entry_sensor;
      sensor::Sensor *min_threshold_exit_sensor;
      sensor::Sensor *exit_roi_height_sensor;
      sensor::Sensor *exit_roi_width_sensor;
      sensor::Sensor *entry_roi_height_sensor;
      sensor::Sensor *entry_roi_width_sensor;
      sensor::Sensor *status_sensor;
      binary_sensor::BinarySensor *presence_sensor;
      text_sensor::TextSensor *version_sensor;
      text_sensor::TextSensor *entry_exit_event_sensor;

      void createEntryAndExitZone();
      void roi_calibration(VL53L1X_ULD distanceSensor, int optimized_zone_0, int optimized_zone_1);
      void calibrateZones(VL53L1X_ULD distanceSensor);
      void setCorrectDistanceSettings(float average_entry_zone_distance, float average_exit_zone_distance);
      void setSensorMode(int sensor_mode, int timing_budget = 0);
      void publishSensorConfiguration(Zone *entry, Zone *exit, bool isMax);
      int getOptimizedValues(int *values, int sum, int size);
      int getSum(int *values, int size);
      void updateCounter(int delta);
      bool calibration_active_{false};
      bool manual_active_{false};
      bool roi_active_{false};
      bool roi_calibration_{false};
      int sensor_offset_calibration_{-1};
      int sensor_xtalk_calibration_{-1};
      int sensor_mode{-1};
      bool advised_sensor_orientation_{true};
      bool sampling_active_{false};
      uint8_t DISTANCES_ARRAY_SIZE{2};
      uint8_t address_ = 0x29;
      bool invert_direction_{false};
      bool restore_values_{false};
      uint64_t max_threshold_percentage_{85};
      uint64_t min_threshold_percentage_{0};
      uint64_t manual_threshold_{2000};
      int number_attempts = 20;
      int timing_budget_{-1};
      int short_distance_threshold = 1300;
      int medium_distance_threshold = 2000;
      int medium_long_distance_threshold = 2700;
      int long_distance_threshold = 3400;
      bool status = false;
      int optimized_zone_0;
      int optimized_zone_1;
    };

  } // namespace roode
} // namespace esphome
