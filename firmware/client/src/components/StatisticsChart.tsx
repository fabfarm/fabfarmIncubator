import {
  Card,
  CardHeader,
  CardBody,
  CardFooter,
  Typography,
} from "@material-tailwind/react";
import Chart from "react-apexcharts";
import dailyTemperatureChart from "../configs/temperature-config";
import dailyHumidityChart from "../configs/humidity-config";

type StatisticsChartProps = {
  color: string;
  chart: typeof dailyTemperatureChart | typeof dailyHumidityChart;
  title: string;
  description: string;
  footer?: string | null;
  xAxisData: any[];
  yAxisData: any[];
};

export function StatisticsChart({
  color,
  chart,
  title,
  description,
  footer,
  xAxisData,
  yAxisData,
}: StatisticsChartProps) {
  return (
    <Card>
      <CardHeader variant="gradient" color={color}>
        {/* @ts-ignore */}
        <Chart
          {...chart}
          options={{
            ...chart.options,
            xaxis: { ...chart.options.xaxis, categories: xAxisData },
          }}
          series={[{ name: chart.series[0].name, data: yAxisData }]}
        />
      </CardHeader>
      <CardBody className="p-6">
        <Typography variant="h6" color="blue-gray">
          {title}
        </Typography>
        <Typography variant="small" className="font-normal text-blue-gray-600">
          {description}
        </Typography>
      </CardBody>
      {footer && (
        <CardFooter className="border-t border-blue-gray-50 px-6 py-5">
          {footer}
        </CardFooter>
      )}
    </Card>
  );
}

export default StatisticsChart;
