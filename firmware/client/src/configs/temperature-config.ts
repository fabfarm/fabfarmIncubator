import chartsConfig from "./chart-config";

const dailyTemperatureChart = {
  type: "line",
  height: 220,
  series: [
    {
      name: "Temperature",
      data: [50, 40, 300, 320, 500, 350, 200, 230, 500],
    },
  ],
  options: {
    ...chartsConfig,
    colors: ["#fff"],
    stroke: {
      lineCap: "round",
    },
    markers: {
      ...chartsConfig.markers,
    },
    xaxis: {
      ...chartsConfig.xaxis,
      categories: [
        "1st",
        "2nd",
        "3rd",
        "4th",
        "5th",
        "6th",
        "7th",
        "8th",
        "9th",
      ],
    },
  },
};

export default dailyTemperatureChart;
