import {
  Card,
  CardHeader,
  CardBody,
  CardFooter,
  Typography,
} from "@material-tailwind/react";

type StatisticsCardProps = {
  color: string;
  icon: any;
  title: string;
  value: string;
  footer?: string | null;
};

export function StatisticsCard({
  color = "blue",
  icon,
  title,
  value,
  footer = null,
}: StatisticsCardProps) {
  return (
    <Card>
      <CardHeader
        variant="gradient"
        color={color}
        className="absolute -mt-4 grid h-16 w-16 place-items-center"
      >
        {icon}
      </CardHeader>
      <CardBody className="p-4 text-right">
        <Typography variant="small" className="font-normal text-blue-gray-600">
          {title}
        </Typography>
        <Typography variant="h4" color="blue-gray">
          {value}
        </Typography>
      </CardBody>
      {footer && (
        <CardFooter className="border-t border-blue-gray-50 p-4">
          {footer}
        </CardFooter>
      )}
    </Card>
  );
}

export default StatisticsCard;
